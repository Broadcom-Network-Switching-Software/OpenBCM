/*******************************************************************************
 *
 * \file field_api.c
 *
 * This file is auto-generated using HSDKGen.py.
 *
 * Edits to this file will be lost when it is regenerated.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <sal/core/libc.h>
#include <shared/field.h>
#include <bcm/policer.h>
#include <bcm/port.h>
#include <bcm/rx.h>
#include <bcm/flowtracker.h>
#include <bcm/subport.h>
#include <bcm/field.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/******************************************************************************
* Global variables
 */
/******************************************************************************
* Public functions
 */

int
bcm_ltsw_field_qualify_InPort(int unit, bcm_field_entry_t entry, bcm_port_t data, bcm_port_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OutPort(int unit, bcm_field_entry_t entry, bcm_port_t data, bcm_port_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOutPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InPorts(int unit, bcm_field_entry_t entry, bcm_pbmp_t data, bcm_pbmp_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInPorts;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PBMP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Drop(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDrop;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcTrunk(int unit, bcm_field_entry_t entry, bcm_trunk_t data, bcm_trunk_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcTrunk;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_TRUNK;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstTrunk(int unit, bcm_field_entry_t entry, bcm_trunk_t data, bcm_trunk_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstTrunk;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_TRUNK;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerL4SrcPort(int unit, bcm_field_entry_t entry, bcm_l4_port_t data, bcm_l4_port_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerL4SrcPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerL4DstPort(int unit, bcm_field_entry_t entry, bcm_l4_port_t data, bcm_l4_port_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerL4DstPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4SrcPort(int unit, bcm_field_entry_t entry, bcm_l4_port_t data, bcm_l4_port_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4SrcPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4DstPort(int unit, bcm_field_entry_t entry, bcm_l4_port_t data, bcm_l4_port_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4DstPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlan(int unit, bcm_field_entry_t entry, bcm_vlan_t data, bcm_vlan_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlan;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanId(int unit, bcm_field_entry_t entry, bcm_vlan_t data, bcm_vlan_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanPri(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanPri;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanCfi(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanCfi;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanPriCfi(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanPriCfi;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlan(int unit, bcm_field_entry_t entry, bcm_vlan_t data, bcm_vlan_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlan;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanId(int unit, bcm_field_entry_t entry, bcm_vlan_t data, bcm_vlan_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanPri(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanPri;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanCfi(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanCfi;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EtherType(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEtherType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Loopback(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopback;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelType(int unit, bcm_field_entry_t entry, bcm_field_TunnelType_t tunnel_type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &tunnel_type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyTunnelType,
            (uint32)tunnel_type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntPriority(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntPriority;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingVlanId(int unit, bcm_field_entry_t entry, bcm_vlan_t data, bcm_vlan_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingVlanId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingVlanId_get(int unit, bcm_field_entry_t entry, bcm_vlan_t *data, bcm_vlan_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingVlanId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Vpn(int unit, bcm_field_entry_t entry, bcm_vpn_t data, bcm_vpn_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVpn;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VPN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Vpn_get(int unit, bcm_field_entry_t entry, bcm_vpn_t *data, bcm_vpn_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVpn;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VPN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpProtocol(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp(int unit, bcm_field_entry_t entry, bcm_ip_t data, bcm_ip_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp(int unit, bcm_field_entry_t entry, bcm_ip_t data, bcm_ip_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DSCP(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDSCP;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Tos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpFlags(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpControl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpControl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpHeaderSize(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpHeaderSize;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ttl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6High(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6High;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6High(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6High;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6Low(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6Low;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6Low(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6Low;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6NextHeader(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6NextHeader;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6TrafficClass(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6TrafficClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6FlowLabel(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6FlowLabel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6HopLimit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6HopLimit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcMac(int unit, bcm_field_entry_t entry, bcm_mac_t data, bcm_mac_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstMac(int unit, bcm_field_entry_t entry, bcm_mac_t data, bcm_mac_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerIpType(int unit, bcm_field_entry_t entry, bcm_field_IpType_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyInnerIpType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingType(int unit, bcm_field_entry_t entry, bcm_field_ForwardingType_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyForwardingType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpType(int unit, bcm_field_entry_t entry, bcm_field_IpType_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyIpType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2Format(int unit, bcm_field_entry_t entry, bcm_field_L2Format_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2Format;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyL2Format,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassPort(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassL2(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassL2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassL3(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassL3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcClassL2(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcClassL2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcClassL3(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcClassL3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcClassField(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcClassField;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstClassL2(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstClassL2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstClassL3(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstClassL3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcCompressionClassId(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcCompressionClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcCompressionClassId_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcCompressionClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstCompressionClassId(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstCompressionClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstCompressionClassId_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstCompressionClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstClassField(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstClassField;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpProtocolCommon(int unit, bcm_field_entry_t entry, bcm_field_IpProtocolCommon_t protocol)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpProtocolCommon;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &protocol;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyIpProtocolCommon,
            (uint32)protocol,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerIpProtocolCommon(int unit, bcm_field_entry_t entry, bcm_field_IpProtocolCommon_t protocol)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpProtocolCommon;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &protocol;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyInnerIpProtocolCommon,
            (uint32)protocol,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3Routable(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3Routable;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpFrag(int unit, bcm_field_entry_t entry, bcm_field_IpFrag_t frag_info)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpFrag;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &frag_info;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyIpFrag,
            (uint32)frag_info,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Vrf(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVrf;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3Ingress(int unit, bcm_field_entry_t entry, uint32 intf_id, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3Ingress;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &intf_id;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ExtensionHeaderType(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyExtensionHeaderType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ExtensionHeaderSubCode(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyExtensionHeaderSubCode;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ExtensionHeader2Type(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyExtensionHeader2Type;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4Ports(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4Ports;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SampledPkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySampledPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6SrhValid(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6SrhValid;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MirrorCopy(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMirrorCopy;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelTerminated(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelTerminated;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsTerminated(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsTerminated;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerSrcIp(int unit, bcm_field_entry_t entry, bcm_ip_t data, bcm_ip_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerSrcIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerDstIp(int unit, bcm_field_entry_t entry, bcm_ip_t data, bcm_ip_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerDstIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerSrcIp6(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerSrcIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerDstIp6(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerDstIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerSrcIp6High(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerSrcIp6High;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerDstIp6High(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerDstIp6High;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerTtl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerTos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerTos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerIpProtocol(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerIpFrag(int unit, bcm_field_entry_t entry, bcm_field_IpFrag_t frag_info)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpFrag;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &frag_info;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyInnerIpFrag,
            (uint32)frag_info,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DosAttack(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDosAttack;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MyStationHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMyStationHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3DestRouteHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3DestRouteHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2CacheHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2CacheHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2StationMove(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2StationMove;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2DestHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2DestHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2SrcStatic(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2SrcStatic;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2SrcHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2SrcHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingVlanValid(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingVlanValid;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3EgressNextHops(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3EgressNextHops;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IcmpTypeCode(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIcmpTypeCode;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VnTag(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVnTag;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntPriority_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntPriority;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelType_get(int unit, bcm_field_entry_t entry, bcm_field_TunnelType_t *tunnel_type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &tunnel_type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyTunnelType,
            &qual_info,
            (uint32 *)tunnel_type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Loopback_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopback;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InPort_get(int unit, bcm_field_entry_t entry, bcm_port_t *data, bcm_port_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OutPort_get(int unit, bcm_field_entry_t entry, bcm_port_t *data, bcm_port_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOutPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InPorts_get(int unit, bcm_field_entry_t entry, bcm_pbmp_t *data, bcm_pbmp_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInPorts;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PBMP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Drop_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDrop;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcTrunk_get(int unit, bcm_field_entry_t entry, bcm_trunk_t *data, bcm_trunk_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcTrunk;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_TRUNK;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstTrunk_get(int unit, bcm_field_entry_t entry, bcm_trunk_t *data, bcm_trunk_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstTrunk;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_TRUNK;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerL4SrcPort_get(int unit, bcm_field_entry_t entry, bcm_l4_port_t *data, bcm_l4_port_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerL4SrcPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerL4DstPort_get(int unit, bcm_field_entry_t entry, bcm_l4_port_t *data, bcm_l4_port_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerL4DstPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4SrcPort_get(int unit, bcm_field_entry_t entry, bcm_l4_port_t *data, bcm_l4_port_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4SrcPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4DstPort_get(int unit, bcm_field_entry_t entry, bcm_l4_port_t *data, bcm_l4_port_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4DstPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlan_get(int unit, bcm_field_entry_t entry, bcm_vlan_t *data, bcm_vlan_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlan;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanId_get(int unit, bcm_field_entry_t entry, bcm_vlan_t *data, bcm_vlan_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanPri_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanPri;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanCfi_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanCfi;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanPriCfi_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanPriCfi;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlan_get(int unit, bcm_field_entry_t entry, bcm_vlan_t *data, bcm_vlan_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlan;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanId_get(int unit, bcm_field_entry_t entry, bcm_vlan_t *data, bcm_vlan_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanPri_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanPri;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanCfi_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanCfi;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EtherType_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEtherType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpProtocol_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp_get(int unit, bcm_field_entry_t entry, bcm_ip_t *data, bcm_ip_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp_get(int unit, bcm_field_entry_t entry, bcm_ip_t *data, bcm_ip_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Tos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DSCP_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDSCP;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpFlags_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpControl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpControl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpHeaderSize_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpHeaderSize;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ttl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6High_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6High;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6High_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6High;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6Low_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6Low;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6Low_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6Low;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6NextHeader_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6NextHeader;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6TrafficClass_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6TrafficClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6FlowLabel_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6FlowLabel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6HopLimit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6HopLimit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcMac_get(int unit, bcm_field_entry_t entry, bcm_mac_t *data, bcm_mac_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstMac_get(int unit, bcm_field_entry_t entry, bcm_mac_t *data, bcm_mac_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerIpType_get(int unit, bcm_field_entry_t entry, bcm_field_IpType_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyInnerIpType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingType_get(int unit, bcm_field_entry_t entry, bcm_field_ForwardingType_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyForwardingType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpType_get(int unit, bcm_field_entry_t entry, bcm_field_IpType_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyIpType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2Format_get(int unit, bcm_field_entry_t entry, bcm_field_L2Format_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2Format;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyL2Format,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassPort_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassL2_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassL2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassL3_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassL3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcClassL2_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcClassL2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcClassL3_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcClassL3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcClassField_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcClassField;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstClassL2_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstClassL2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstClassL3_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstClassL3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstClassField_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstClassField;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpProtocolCommon_get(int unit, bcm_field_entry_t entry, bcm_field_IpProtocolCommon_t *protocol)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpProtocolCommon;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &protocol;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyIpProtocolCommon,
            &qual_info,
            (uint32 *)protocol));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerIpProtocolCommon_get(int unit, bcm_field_entry_t entry, bcm_field_IpProtocolCommon_t *protocol)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpProtocolCommon;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &protocol;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyInnerIpProtocolCommon,
            &qual_info,
            (uint32 *)protocol));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3Routable_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3Routable;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpFrag_get(int unit, bcm_field_entry_t entry, bcm_field_IpFrag_t *frag_info)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpFrag;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &frag_info;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyIpFrag,
            &qual_info,
            (uint32 *)frag_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Vrf_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVrf;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3Ingress_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3Ingress;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ExtensionHeaderType_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyExtensionHeaderType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ExtensionHeaderSubCode_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyExtensionHeaderSubCode;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ExtensionHeader2Type_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyExtensionHeader2Type;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SampledPkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySampledPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6SrhValid_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6SrhValid;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4Ports_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4Ports;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MirrorCopy_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMirrorCopy;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelTerminated_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelTerminated;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsTerminated_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsTerminated;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerSrcIp_get(int unit, bcm_field_entry_t entry, bcm_ip_t *data, bcm_ip_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerSrcIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerDstIp_get(int unit, bcm_field_entry_t entry, bcm_ip_t *data, bcm_ip_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerDstIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerSrcIp6_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerSrcIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerDstIp6_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerDstIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerSrcIp6High_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerSrcIp6High;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerDstIp6High_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerDstIp6High;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerTtl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerTos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerTos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerIpProtocol_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerIpFrag_get(int unit, bcm_field_entry_t entry, bcm_field_IpFrag_t *frag_info)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpFrag;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &frag_info;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyInnerIpFrag,
            &qual_info,
            (uint32 *)frag_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DosAttack_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDosAttack;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MyStationHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMyStationHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3DestRouteHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3DestRouteHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2CacheHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2CacheHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2StationMove_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2StationMove;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2DestHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2DestHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2SrcStatic_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2SrcStatic;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2SrcHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2SrcHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingVlanValid_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingVlanValid;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3EgressNextHops_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3EgressNextHops;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IcmpTypeCode_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIcmpTypeCode;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VnTag_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVnTag;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpTunnelHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpTunnelHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpTunnelHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpTunnelHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Hit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Hit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Hit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Hit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Hit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Hit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Hit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Hit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabel(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabel_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelTtl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelTtl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelBos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelBos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelBos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelBos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelExp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelExp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelExp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelExp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelId(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelId_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsControlWord(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsControlWord;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsControlWord_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsControlWord;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7AHashUpper(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7AHashUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7AHashUpper_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7AHashUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7AHashLower(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7AHashLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7AHashLower_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7AHashLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7BHashUpper(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7BHashUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7BHashUpper_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7BHashUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7BHashLower(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7BHashLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7BHashLower_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7BHashLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelAction(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelAction;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelAction_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelAction;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelAction32(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelAction;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelAction32_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelAction;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsControlWordValid(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsControlWordValid;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsControlWordValid_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsControlWordValid;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressClass(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressClass_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressClassL3Interface(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressClassL3Interface;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressClassL3Interface_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressClassL3Interface;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CpuQueue(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCpuQueue;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CpuQueue_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCpuQueue;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2Learn(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2Learn;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2Learn_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2Learn;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EcnValue(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEcnValue;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EcnValue_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEcnValue;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsTerminatedHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsTerminatedHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsTerminatedHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsTerminatedHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3SrcRouteHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3SrcRouteHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3SrcRouteHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3SrcRouteHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsTerminatedValue(int unit, bcm_field_entry_t entry, uint64 data, uint64 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsTerminatedValue;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT64;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsTerminatedValue_get(int unit, bcm_field_entry_t entry, uint64 *data, uint64 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsTerminatedValue;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT64;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Ttl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Ttl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Bos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Bos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Exp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Exp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Id(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel1Id_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel1Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Ttl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Ttl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Bos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Bos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Exp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Exp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Id(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel2Id_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel2Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3Ttl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3Ttl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3Bos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3Bos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3Exp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3Exp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3Id(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel3Id_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel3Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Svtag(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySvtag;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Svtag_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySvtag;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressClassField(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressClassField;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressClassField_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressClassField;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanNetworkId(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanNetworkId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanNetworkId_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanNetworkId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanFlags(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanFlags_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IcmpError(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIcmpError;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IcmpError_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIcmpError;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpSenderIp4(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpSenderIp4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpSenderIp4_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpSenderIp4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpTargetIp4(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpTargetIp4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpTargetIp4_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpTargetIp4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpOpcode(int unit, bcm_field_entry_t entry, bcm_field_ArpOpcode_t data)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpOpcode;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyArpOpcode,
            (uint32)data,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpOpcode_get(int unit, bcm_field_entry_t entry, bcm_field_ArpOpcode_t *data)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpOpcode;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyArpOpcode,
            &qual_info,
            (uint32 *)data));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpFragNonOrFirst(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpFragNonOrFirst;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpFragNonOrFirst_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpFragNonOrFirst;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PacketLength(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPacketLength;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PacketLength_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPacketLength;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GroupClass(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGroupClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GroupClass_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGroupClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_UdfClass(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyUdfClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_UdfClass_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyUdfClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceLookupClassPort(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceLookupClassPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceLookupClassPort_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceLookupClassPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceIngressKeySelectClassPort(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceIngressKeySelectClassPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceIngressKeySelectClassPort_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceIngressKeySelectClassPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_BfdYourDiscriminator(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyBfdYourDiscriminator;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_BfdYourDiscriminator_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyBfdYourDiscriminator;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL2MulticastGroup(int unit, bcm_field_entry_t entry, bcm_multicast_t group, bcm_multicast_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL2MulticastGroup;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MULTICAST;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &group;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL2MulticastGroup_get(int unit, bcm_field_entry_t entry, bcm_multicast_t *group, bcm_multicast_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL2MulticastGroup;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MULTICAST;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &group;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3MulticastGroup(int unit, bcm_field_entry_t entry, bcm_multicast_t group, bcm_multicast_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3MulticastGroup;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MULTICAST;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &group;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3MulticastGroup_get(int unit, bcm_field_entry_t entry, bcm_multicast_t *group, bcm_multicast_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3MulticastGroup;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MULTICAST;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &group;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntCongestionNotification(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntCongestionNotification;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntCongestionNotification_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntCongestionNotification;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelPayload(int unit, bcm_field_entry_t entry, uint64 data, uint64 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelPayload;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT64;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingLabelPayload_get(int unit, bcm_field_entry_t entry, uint64 *data, uint64 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingLabelPayload;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT64;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanActionRange(int unit, bcm_field_entry_t entry, bcm_vlan_t data, bcm_vlan_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanActionRange;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanActionRange_get(int unit, bcm_field_entry_t entry, bcm_vlan_t *data, bcm_vlan_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanActionRange;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanActionRange(int unit, bcm_field_entry_t entry, bcm_vlan_t data, bcm_vlan_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanActionRange;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanActionRange_get(int unit, bcm_field_entry_t entry, bcm_vlan_t *data, bcm_vlan_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanActionRange;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_VLAN;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanHeaderBits8_31(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanHeaderBits8_31;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanHeaderBits8_31_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanHeaderBits8_31;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanHeaderBits56_63(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanHeaderBits56_63;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanHeaderBits56_63_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanHeaderBits56_63;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpProtocolClass(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpProtocolClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpProtocolClass_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpProtocolClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EtherTypeClass(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEtherTypeClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EtherTypeClass_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEtherTypeClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4SrcPortClass(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4SrcPortClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4SrcPortClass_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4SrcPortClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4DstPortClass(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4DstPortClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L4DstPortClass_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL4DstPortClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIpClass(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIpClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIpClass_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIpClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIpClassMsbNibble(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIpClassMsbNibble;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIpClassMsbNibble_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIpClassMsbNibble;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIpClassLower(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIpClassLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIpClassLower_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIpClassLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIpClassUpper(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIpClassUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIpClassUpper_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIpClassUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6Class(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6Class;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6Class_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6Class;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6ClassMsbNibble(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6ClassMsbNibble;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6ClassMsbNibble_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6ClassMsbNibble;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6ClassLower(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6ClassLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6ClassLower_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6ClassLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6ClassUpper(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6ClassUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcIp6ClassUpper_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcIp6ClassUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIpClass(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIpClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIpClass_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIpClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIpClassMsbNibble(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIpClassMsbNibble;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIpClassMsbNibble_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIpClassMsbNibble;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIpClassLower(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIpClassLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIpClassLower_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIpClassLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIpClassUpper(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIpClassUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIpClassUpper_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIpClassUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6Class(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6Class;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6Class_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6Class;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6ClassMsbNibble(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6ClassMsbNibble;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6ClassMsbNibble_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6ClassMsbNibble;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6ClassLower(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6ClassLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6ClassLower_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6ClassLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6ClassUpper(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6ClassUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstIp6ClassUpper_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstIp6ClassUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpClassZero(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpClassZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpClassZero_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpClassZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TosClassZero(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTosClassZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TosClassZero_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTosClassZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TtlClassZero(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTtlClassZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TtlClassZero_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTtlClassZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpClassOne(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpClassOne;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpClassOne_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpClassOne;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TosClassOne(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTosClassOne;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TosClassOne_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTosClassOne;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TtlClassOne(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTtlClassOne;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TtlClassOne_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTtlClassOne;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackQueue(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackQueue;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackQueue_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackQueue;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackSrcGport(int unit, bcm_field_entry_t entry, bcm_gport_t port_id, bcm_gport_t port_mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackSrcGport;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_GPORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &port_id;
    qual_info.qual_mask = (void *) &port_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackSrcGport_get(int unit, bcm_field_entry_t entry, bcm_gport_t *port_id, bcm_gport_t *port_mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackSrcGport;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_GPORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &port_id;
    qual_info.qual_mask = (void *) &port_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackCpuMasqueradePktProfile(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackCpuMasqueradePktProfile;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackCpuMasqueradePktProfile_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackCpuMasqueradePktProfile;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackTrafficClass(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackTrafficClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackTrafficClass_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackTrafficClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackPacketProcessingPort(int unit, bcm_field_entry_t entry, bcm_gport_t port_id, bcm_gport_t port_mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackPacketProcessingPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_GPORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &port_id;
    qual_info.qual_mask = (void *) &port_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopBackPacketProcessingPort_get(int unit, bcm_field_entry_t entry, bcm_gport_t *port_id, bcm_gport_t *port_mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopBackPacketProcessingPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_GPORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &port_id;
    qual_info.qual_mask = (void *) &port_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SystemPortBitmap(int unit, bcm_field_entry_t entry, bcm_pbmp_t data, bcm_pbmp_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySystemPortBitmap;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PBMP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SystemPortBitmap_get(int unit, bcm_field_entry_t entry, bcm_pbmp_t *data, bcm_pbmp_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySystemPortBitmap;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PBMP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DevicePortBitmap(int unit, bcm_field_entry_t entry, bcm_pbmp_t data, bcm_pbmp_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDevicePortBitmap;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PBMP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DevicePortBitmap_get(int unit, bcm_field_entry_t entry, bcm_pbmp_t *data, bcm_pbmp_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDevicePortBitmap;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PBMP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2PayLoad(int unit, bcm_field_entry_t entry, uint32 length, const uint8 *data, const uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2PayLoad;
    qual_info.flags = BCM_FIELD_F_ARRAY;
    qual_info.depth = length;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2PayLoad_get(int unit, bcm_field_entry_t entry, uint32 length, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2PayLoad;
    qual_info.flags = BCM_FIELD_F_ARRAY;
    qual_info.depth = length;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TosLower4Bits(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTosLower4Bits;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TosLower4Bits_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTosLower4Bits;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MyStation2Hit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMyStation2Hit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MyStation2Hit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMyStation2Hit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressClassL3InterfaceTunnel(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressClassL3InterfaceTunnel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressClassL3InterfaceTunnel_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressClassL3InterfaceTunnel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SysHdrType(int unit, bcm_field_entry_t entry, bcm_field_sys_hdr_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySysHdrType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifySysHdrType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SysHdrType_get(int unit, bcm_field_entry_t entry, bcm_field_sys_hdr_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySysHdrType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifySysHdrType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpTunnelTtlClassZero(int unit, bcm_field_entry_t entry, bcm_class_t data, bcm_class_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpTunnelTtlClassZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_NetworkTagType(int unit, bcm_field_entry_t entry, bcm_field_network_tag_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyNetworkTagType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyNetworkTagType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_NetworkTagType_get(int unit, bcm_field_entry_t entry, bcm_field_network_tag_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyNetworkTagType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyNetworkTagType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2PktType(int unit, bcm_field_entry_t entry, bcm_field_l2_pkt_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2PktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyL2PktType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2PktType_get(int unit, bcm_field_entry_t entry, bcm_field_l2_pkt_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2PktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyL2PktType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PktType(int unit, bcm_field_entry_t entry, bcm_field_pkt_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyPktType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PktType_get(int unit, bcm_field_entry_t entry, bcm_field_pkt_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyPktType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsCwPresent(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsCwPresent;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsCwPresent_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsCwPresent;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayL2PktType(int unit, bcm_field_entry_t entry, bcm_field_l2_pkt_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayL2PktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyOverlayL2PktType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayL2PktType_get(int unit, bcm_field_entry_t entry, bcm_field_l2_pkt_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayL2PktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyOverlayL2PktType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayPktType(int unit, bcm_field_entry_t entry, bcm_field_pkt_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayPktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyOverlayPktType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayPktType_get(int unit, bcm_field_entry_t entry, bcm_field_pkt_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayPktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyOverlayPktType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_FrontPanelPkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyFrontPanelPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_FrontPanelPkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyFrontPanelPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterPriTaggedPkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterPriTaggedPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterPriTaggedPkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterPriTaggedPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpChecksumOkPkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpChecksumOkPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpChecksumOkPkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpChecksumOkPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntPktFinalHop(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntPktFinalHop;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntPktFinalHop_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntPktFinalHop;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpTunnelTtlClassZero_get(int unit, bcm_field_entry_t entry, bcm_class_t *data, bcm_class_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpTunnelTtlClassZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_CLASS;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpTunnelTtl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpTunnelTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpTunnelTtl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpTunnelTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassL3Tunnel(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassL3Tunnel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassL3Tunnel_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassL3Tunnel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6PktTrafficClass(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6PktTrafficClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip6PktTrafficClass_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp6PktTrafficClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip4Tos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp4Tos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip4Tos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp4Tos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RangeCheckGroup(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRangeCheckGroup;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RangeCheckGroup_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRangeCheckGroup;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueTagType(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueTagType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueTagType_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueTagType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueTagHigh(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueTagHigh;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueTagHigh_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueTagHigh;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueTagLow(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueTagLow;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueTagLow_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueTagLow;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DosAttackEvents(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDosAttackEvents;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DosAttackEvents_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDosAttackEvents;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip4Length(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp4Length;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Ip4Length_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIp4Length;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanGbpPresent(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanGbpPresent;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VxlanGbpPresent_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVxlanGbpPresent;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueObject1(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueObject1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueObject1_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueObject1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueObject2(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueObject2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueObject2_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueObject2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueObject3(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueObject3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueObject3_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueObject3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueObject4(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueObject4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OpaqueObject4_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOpaqueObject4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3EgressNextHopsUnderlay(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3EgressNextHopsUnderlay;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3EgressNextHopsUnderlay_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3EgressNextHopsUnderlay;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressClassPort(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressClassPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressClassPort_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressClassPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_INTProbeMarker1(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyINTProbeMarker1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_INTProbeMarker1_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyINTProbeMarker1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_INTProbeMarker2(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyINTProbeMarker2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_INTProbeMarker2_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyINTProbeMarker2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IgmpType(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIgmpType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IgmpType_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIgmpType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2IngressInterface(int unit, bcm_field_entry_t entry, uint32 intf_id, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2IngressInterface;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &intf_id;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2IngressInterface_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2IngressInterface;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3SrcHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3SrcHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3SrcHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3SrcHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3DestHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3DestHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3DestHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3DestHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerPriTaggedPkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerPriTaggedPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerPriTaggedPkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerPriTaggedPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_HiGig3Class(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyHiGig3Class;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_HiGig3Class_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyHiGig3Class;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpExtensionHdrNum(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpExtensionHdrNum;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpExtensionHdrNum_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpExtensionHdrNum;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL2InterfaceOpaqueCtrlId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL2InterfaceOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL2InterfaceOpaqueCtrlId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL2InterfaceOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DevicePortOpaqueCommand(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDevicePortOpaqueCommand;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DevicePortOpaqueCommand_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDevicePortOpaqueCommand;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressInterfaceClass(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressInterfaceClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressInterfaceClass_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressInterfaceClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL3InterfaceOpaqueCtrlId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL3InterfaceOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL3InterfaceOpaqueCtrlId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL3InterfaceOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SvpOpaqueCtrlId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySvpOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SvpOpaqueCtrlId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySvpOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VpnOpaqueCtrlId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVpnOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VpnOpaqueCtrlId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVpnOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2SrcDiscard(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2SrcDiscard;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2SrcDiscard_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2SrcDiscard;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltEtherType(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltEtherType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltEtherType_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltEtherType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltSrcIp(int unit, bcm_field_entry_t entry, bcm_ip_t data, bcm_ip_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltSrcIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltSrcIp_get(int unit, bcm_field_entry_t entry, bcm_ip_t *data, bcm_ip_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltSrcIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltSrcIp6(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltSrcIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltSrcIp6_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltSrcIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltDstIp(int unit, bcm_field_entry_t entry, bcm_ip_t data, bcm_ip_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltDstIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltDstIp_get(int unit, bcm_field_entry_t entry, bcm_ip_t *data, bcm_ip_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltDstIp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltDstIp6(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltDstIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltDstIp6_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltDstIp6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltIpProtocol(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltIpProtocol_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltL4SrcPort(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltL4SrcPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltL4SrcPort_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltL4SrcPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltL4DstPort(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltL4DstPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelAltL4DstPort_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelAltL4DstPort;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EffectiveTtl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEffectiveTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EffectiveTtl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEffectiveTtl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_FlexStateResultA0(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyFlexStateResultA0;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_FlexStateResultA0_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyFlexStateResultA0;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_FlexStateResultA1(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyFlexStateResultA1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_FlexStateResultA1_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyFlexStateResultA1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LastIpProtocol(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLastIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LastIpProtocol_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLastIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4Ttl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4Ttl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4Bos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4Bos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4Exp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4Exp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4Id(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel4Id_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel4Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5Ttl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5Ttl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5Bos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5Bos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5Exp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5Exp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5Id(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel5Id_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel5Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6Ttl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6Ttl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6Bos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6Bos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6Exp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6Exp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6Id(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel6Id_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel6Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7Ttl(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7Ttl_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7Ttl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7Bos(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7Bos_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7Bos;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7Exp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7Exp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7Exp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7Id(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsLabel7Id_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsLabel7Id;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte1(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte1_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte2(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte2_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte3(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte3_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte4(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte4_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte5(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte5;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte5_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte5;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte6(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte6_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte7(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte7;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte7_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte7;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte8(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte8;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L5PayloadByte8_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL5PayloadByte8;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte1(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte1_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte2(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte2_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte2;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte3(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte3_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte3;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte4(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte4_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte4;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte5(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte5;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte5_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte5;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte6(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte6_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte7(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte7;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte7_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte7;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte8(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte8;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterL5PayloadByte8_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL5PayloadByte8;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpSrcMac(int unit, bcm_field_entry_t entry, bcm_mac_t data, bcm_mac_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpSrcMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpSrcMac_get(int unit, bcm_field_entry_t entry, bcm_mac_t *data, bcm_mac_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpSrcMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpDstMac(int unit, bcm_field_entry_t entry, bcm_mac_t data, bcm_mac_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpDstMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ArpDstMac_get(int unit, bcm_field_entry_t entry, bcm_mac_t *data, bcm_mac_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyArpDstMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2EgressInterface(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2EgressInterface;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2EgressInterface_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2EgressInterface;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Stg(int unit, bcm_field_entry_t entry, bcm_stg_t data, bcm_stg_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyStg;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_STG;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Stg_get(int unit, bcm_field_entry_t entry, bcm_stg_t *data, bcm_stg_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyStg;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_STG;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CosQueueNum(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCosQueueNum;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CosQueueNum_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCosQueueNum;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CopyToCpu(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCopyToCpu;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CopyToCpu_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCopyToCpu;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CopyToCpuTruncate(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCopyToCpuTruncate;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CopyToCpuTruncate_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCopyToCpuTruncate;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_McastPkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMcastPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_McastPkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMcastPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_UntagPkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyUntagPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_UntagPkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyUntagPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DoNotModify(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDoNotModify;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DoNotModify_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDoNotModify;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TxPortType(int unit, bcm_field_entry_t entry, bcm_field_PortType_t port_type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTxPortType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &port_type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyTxPortType,
            (uint32)port_type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TxPortType_get(int unit, bcm_field_entry_t entry, bcm_field_PortType_t *port_type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTxPortType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &port_type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyTxPortType,
            &qual_info,
            (uint32 *)port_type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RxPortType(int unit, bcm_field_entry_t entry, bcm_field_PortType_t port_type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRxPortType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &port_type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyRxPortType,
            (uint32)port_type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RxPortType_get(int unit, bcm_field_entry_t entry, bcm_field_PortType_t *port_type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRxPortType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &port_type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyRxPortType,
            &qual_info,
            (uint32 *)port_type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PtpMsgType(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpMsgType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PtpMsgType_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpMsgType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PtpVersion(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpVersion;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PtpVersion_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpVersion;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VlanMembershipCheck(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVlanMembershipCheck;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_VlanMembershipCheck_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyVlanMembershipCheck;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanTagPreserve(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanTagPreserve;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanTagPreserve_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanTagPreserve;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanTagPreserve(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanTagPreserve;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanTagPreserve_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanTagPreserve;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassMpls(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassMpls;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InterfaceClassMpls_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInterfaceClassMpls;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2InterfaceMatch(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2InterfaceMatch;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2InterfaceMatch_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2InterfaceMatch;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ProtocolPktOpaqueCtrlId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyProtocolPktOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ProtocolPktOpaqueCtrlId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyProtocolPktOpaqueCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LookupClassField(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLookupClassField;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LookupClassField_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLookupClassField;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ExactMatchOpaqueObject0(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyExactMatchOpaqueObject0;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ExactMatchOpaqueObject0_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyExactMatchOpaqueObject0;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IcmpType(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIcmpType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IcmpType_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIcmpType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanRangeMatch(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanRangeMatch;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterVlanRangeMatch_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterVlanRangeMatch;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanRangeMatch(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanRangeMatch;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerVlanRangeMatch_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerVlanRangeMatch;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderDstMac(int unit, bcm_field_entry_t entry, bcm_mac_t data, bcm_mac_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderDstMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderDstMac_get(int unit, bcm_field_entry_t entry, bcm_mac_t *data, bcm_mac_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderDstMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderSrcMac(int unit, bcm_field_entry_t entry, bcm_mac_t data, bcm_mac_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderSrcMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderSrcMac_get(int unit, bcm_field_entry_t entry, bcm_mac_t *data, bcm_mac_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderSrcMac;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderSip(int unit, bcm_field_entry_t entry, bcm_ip_t data, bcm_ip_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderSip;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderDip(int unit, bcm_field_entry_t entry, bcm_ip_t data, bcm_ip_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderDip;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderSip_get(int unit, bcm_field_entry_t entry, bcm_ip_t *data, bcm_ip_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderSip;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderDip_get(int unit, bcm_field_entry_t entry, bcm_ip_t *data, bcm_ip_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderDip;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderSip6(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderSip6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderDip6(int unit, bcm_field_entry_t entry, bcm_ip6_t data, bcm_ip6_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderDip6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderSip6_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderSip6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderDip6_get(int unit, bcm_field_entry_t entry, bcm_ip6_t *data, bcm_ip6_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderDip6;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderIpProtocol(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TunnelHeaderIpProtocol_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelHeaderIpProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaGlobalNameSpace(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaGlobalNameSpace;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaGlobalNameSpace_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaGlobalNameSpace;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaRequestVector(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaRequestVector;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaRequestVector_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaRequestVector;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaFlags(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaFlags_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaMaxLength(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaMaxLength;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaMaxLength_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaMaxLength;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RecircleHdrCode(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRecircleHdrCode;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RecircleHdrCode_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRecircleHdrCode;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MacSecOpaque(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMacSecOpaque;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MacSecOpaque_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMacSecOpaque;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MirrorOnDrop(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMirrorOnDrop;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MirrorOnDrop_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMirrorOnDrop;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MirrorOnDropReason(int unit, bcm_field_entry_t entry, bcm_field_mirror_on_drop_reason_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMirrorOnDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyMirrorOnDropReason,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MirrorOnDropReason_get(int unit, bcm_field_entry_t entry, bcm_field_mirror_on_drop_reason_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMirrorOnDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyMirrorOnDropReason,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaHopCount(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaHopCount;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaHopCount_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaHopCount;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaHopLimit(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaHopLimit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaHopLimit_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaHopLimit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CopyToCpuReasonLow(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCopyToCpuReasonLow;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CopyToCpuReasonLow_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCopyToCpuReasonLow;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7CHashUpper(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7CHashUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7CHashUpper_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7CHashUpper;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7CHashLower(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7CHashLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Rtag7CHashLower_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRtag7CHashLower;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayNetworkTagType(int unit, bcm_field_entry_t entry, bcm_field_network_tag_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayNetworkTagType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyOverlayNetworkTagType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayNetworkTagType_get(int unit, bcm_field_entry_t entry, bcm_field_network_tag_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayNetworkTagType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyOverlayNetworkTagType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpOptionHdrType(int unit, bcm_field_entry_t entry, bcm_field_ip_option_hdr_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpOptionHdrType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyIpOptionHdrType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpOptionHdrType_get(int unit, bcm_field_entry_t entry, bcm_field_ip_option_hdr_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpOptionHdrType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyIpOptionHdrType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayIpOptionHdrType(int unit, bcm_field_entry_t entry, bcm_field_ip_option_hdr_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayIpOptionHdrType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyOverlayIpOptionHdrType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayIpOptionHdrType_get(int unit, bcm_field_entry_t entry, bcm_field_ip_option_hdr_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayIpOptionHdrType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyOverlayIpOptionHdrType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayEgressClass(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayEgressClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OverlayEgressClass_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayEgressClass;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressDOPTrigger(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressDOPTrigger;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressDOPTrigger_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressDOPTrigger;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressTunnelIpHeaderFlowLabel(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressTunnelIpHeaderFlowLabel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressTunnelIpHeaderFlowLabel_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressTunnelIpHeaderFlowLabel;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressTunnelIpHeaderEcn(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressTunnelIpHeaderEcn;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressTunnelIpHeaderEcn_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressTunnelIpHeaderEcn;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressTunnelIpHeaderDscp(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressTunnelIpHeaderDscp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressTunnelIpHeaderDscp_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressTunnelIpHeaderDscp;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpExtHdr2Protocol(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpExtHdr2Protocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpExtHdr2Protocol_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpExtHdr2Protocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressPktQueueNotification(int unit, bcm_field_entry_t entry, bcm_field_pkt_queue_notif_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressPktQueueNotification;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyEgressPktQueueNotification,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressPktQueueNotification_get(int unit, bcm_field_entry_t entry, bcm_field_pkt_queue_notif_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressPktQueueNotification;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyEgressPktQueueNotification,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressMcastPktReason(int unit, bcm_field_entry_t entry, bcm_field_mcast_pkt_reason_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressMcastPktReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyEgressMcastPktReason,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressMcastPktReason_get(int unit, bcm_field_entry_t entry, bcm_field_mcast_pkt_reason_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressMcastPktReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyEgressMcastPktReason,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2EgrIntfEfpCtrlId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2EgrIntfEfpCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2EgrIntfEfpCtrlId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2EgrIntfEfpCtrlId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntFlags(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntFlags_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntUdpChecksumEqualsZero(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntUdpChecksumEqualsZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntUdpChecksumEqualsZero_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntUdpChecksumEqualsZero;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntMetadataHdrType(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntMetadataHdrType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntMetadataHdrType_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntMetadataHdrType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CompressionDstHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCompressionDstHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CompressionDstHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCompressionDstHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CompressionSrcHit(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCompressionSrcHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CompressionSrcHit_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCompressionSrcHit;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_HiGig3Pkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyHiGig3Pkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_HiGig3Pkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyHiGig3Pkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopbackLagFailoverPkt(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackLagFailoverPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopbackLagFailoverPkt_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackLagFailoverPkt;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingOuterVlanPri(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingOuterVlanPri;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingOuterVlanPri_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingOuterVlanPri;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingOuterTpid(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingOuterTpid;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_ForwardingOuterTpid_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingOuterTpid;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_BfdMultiplierLen(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyBfdMultiplierLen;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_BfdVerFlags(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyBfdVerFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeFlags(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeNetworkId(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeNetworkId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeProtocol(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeReserved0(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeReserved0;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeReserved1(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeReserved1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaAttrOpaqData(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaAttrOpaqData;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IgmpMaxRespTime(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIgmpMaxRespTime;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsTopHeader(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsTopHeader;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsVcLabelHeader(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsVcLabelHeader;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpAckNum(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpAckNum;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpSequenceNum(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpSequenceNum;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_UdpLength(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyUdpLength;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PtpReserved1(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpReserved1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PtpTransportSpec(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpTransportSpec;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_BfdMultiplierLen_get(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyBfdMultiplierLen;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_BfdVerFlags_get(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyBfdVerFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeFlags_get(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeNetworkId_get(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeNetworkId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeProtocol_get(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeProtocol;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeReserved0_get(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeReserved0;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_GpeReserved1_get(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyGpeReserved1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaAttrOpaqData_get(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaAttrOpaqData;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IgmpMaxRespTime_get(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIgmpMaxRespTime;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsTopHeader_get(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsTopHeader;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsVcLabelHeader_get(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsVcLabelHeader;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpAckNum_get(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpAckNum;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_TcpSequenceNum_get(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyTcpSequenceNum;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_UdpLength_get(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyUdpLength;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PtpReserved1_get(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpReserved1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PtpTransportSpec_get(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpTransportSpec;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2SrcMiscClassId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2SrcMiscClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2SrcMiscClassId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2SrcMiscClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2DstMiscClassId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2DstMiscClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2DstMiscClassId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2DstMiscClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3SrcMiscClassId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3SrcMiscClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3SrcMiscClassId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3SrcMiscClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3DstMiscClassId(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3DstMiscClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3DstMiscClassId_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3DstMiscClassId;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpFragInfo(int unit, bcm_field_entry_t entry, uint16 data, uint16 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpFragInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IpFragInfo_get(int unit, bcm_field_entry_t entry, uint16 *data, uint16 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIpFragInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RawResidenceTime(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRawResidenceTime;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RawResidenceTime_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRawResidenceTime;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaNextHdr(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaNextHdr;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaNextHdr_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaNextHdr;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaVersion(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaVersion;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IfaVersion_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIfaVersion;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL2InterfaceOpaqueCtrlId1(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL2InterfaceOpaqueCtrlId1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL2InterfaceOpaqueCtrlId1_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL2InterfaceOpaqueCtrlId1;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL2InterfaceFlexDigestCtrlIdA(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL2InterfaceFlexDigestCtrlIdA;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL2InterfaceFlexDigestCtrlIdA_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL2InterfaceFlexDigestCtrlIdA;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL2InterfaceFlexDigestCtrlIdB(int unit, bcm_field_entry_t entry, uint8 data, uint8 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL2InterfaceFlexDigestCtrlIdB;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressL2InterfaceFlexDigestCtrlIdB_get(int unit, bcm_field_entry_t entry, uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressL2InterfaceFlexDigestCtrlIdB;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RecircleHdrOpaqueCtrlIdC(int unit, bcm_field_entry_t entry, int data, int mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRecircleHdrOpaqueCtrlIdC;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_INT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RecircleHdrOpaqueCtrlIdC_get(int unit, bcm_field_entry_t entry, int *data, int *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRecircleHdrOpaqueCtrlIdC;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_INT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingControlWord(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingControlWord;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsForwardingControlWord_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsForwardingControlWord;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsCtrlPktType(int unit, bcm_field_entry_t entry, bcm_field_mpls_ctrl_pkt_type_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsCtrlPktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(unit,
            bcmFieldQualifyMplsCtrlPktType,
            (uint32)type,
            &qual_info));


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MplsCtrlPktType_get(int unit, bcm_field_entry_t entry, bcm_field_mpls_ctrl_pkt_type_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMplsCtrlPktType;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(unit,
            bcmFieldQualifyMplsCtrlPktType,
            &qual_info,
            (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

