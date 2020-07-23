/*
 *
 * $Id: cint_field_ipv6_ext_hdr.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_ipv6_ext_hdr.c
 * Purpose: Installs a FG that qualifys on IPv6 packet with at least one extension header
 *
 * Usage:
 * Enable soc property bcm886xx_ipv6_ext_hdr_enable=1
 */

bcm_field_entry_t ipv6_ext_hdr_ent;

int cint_field_ipv6_ext_hdr_main(int unit, bcm_field_group_t grp, int group_priority)
{
    int result = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);

    result = bcm_field_group_create_id(unit, qset, group_priority, grp);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_group_create_id\r\n", result);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_group_action_set\r\n", result);
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &ipv6_ext_hdr_ent);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_entry_create\r\n", result);
        return result;
    }

    result = bcm_field_qualify_IpType(unit, ipv6_ext_hdr_ent, bcmFieldIpTypeIpv6OneExtHdr);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_qualify_IpType\r\n", result);
        return result;
    }

    result = bcm_field_action_add(unit, ipv6_ext_hdr_ent, bcmFieldActionDropPrecedence, 0x2, 0x3);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_action_add\r\n", result);
        return result;
    }

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_group_install\r\n", result);
        return result;
    }

    return result;
}

int cint_field_ipv6_ext_hdr_destroy(int unit, bcm_field_group_t grp)
{
    int result = BCM_E_NONE;
    result = bcm_field_entry_destroy(unit, ipv6_ext_hdr_ent);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_entry_destroy\r\n", result);
        return result;
    }
    result = bcm_field_group_destroy(unit, grp);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_group_destroy\r\n", result);
        return result;
    }
    return result;
}

