/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  
 * File: cint_field_ipv6_extension_header.c 
 * Purpose: Example of how to use IPv6 extension header as qualifier in FP. 
 *  
 * In order to enable processing of IPv6 extension headers, it is necessary 
 * to set a soc property to indicate it. The reason for not enabling it by 
 * default is that the parser has a limited number of resources (macros) and 
 * it cannot be configured to parse all types of packets within the same 
 * configuration. It is the user's choice which types of packets to enable. 
 * 
 * CINT Usage:  
 * 
 *  1. Set soc property:
 *      bcm886xx_ipv6_ext_hdr_enable=1
 *  
 *  2. Run:
 *      cint cint_field_ipv6_extension_header.c
 *      cint
 *      field_ipv6_extension_header_example(int unit);
 */

int grp;
bcm_field_entry_t ent;


int field_ipv6_extension_header_setup(/* in */ int unit,
                                      /* in */ int group_priority,
                                      /* out */ bcm_field_group_t *group)
{
    int result = 0;

    bcm_field_qset_t qset;
    bcm_field_aset_t aset;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp6);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyExtensionHeaderType);
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
     
    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        return result;
    }

    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    result = bcm_field_qualify_EtherType(unit, ent, 0x86DD, 0xffff);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_EtherType $rv %d\n");
        return result;
    }

    result = bcm_field_qualify_IpProtocol(unit, ent, 0x0 /* Hop-by-Hop */, 0xff);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_IpProtocol $rv\n");
        return result;
    }
     
    result = bcm_field_qualify_ExtensionHeaderType(unit, ent, 0x11 /* UDP */, 0xff);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_ExtensionHeaderType $rv\n");
        return result;
    }

    result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }
    
    result = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    *group = grp;

    return result;

} /* field_ipv6_extension_header_setup */


int field_ipv6_extension_header_destroy(/* in */ int unit)
{
    int result = 0;

    result = bcm_field_entry_destroy(unit, ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_destroy entry %d\n", ent);
        return result;
    }

    result = bcm_field_group_destroy(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_destroy group %d\n", grp);
        return result;
    }

    return result;

} /* field_ipv6_extension_header_destroy */


int field_ipv6_extension_header_example(/* in */ int unit)
{
    int result;
    int group_priority = 10;
    bcm_field_group_t group;

    result = field_ipv6_extension_header_setup(unit, group_priority, &group);
    if (BCM_E_NONE != result) {
        printf("Error in field_ipv6_extension_header_setup\n");
    }

    return result;
}
