/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  
 * File: cint_field_drop_static_sa_transplant.c 
 * Purpose: Example of how to use Learn_or_Transplant qualifier in Ingress FP. 
 *  
 * When inserting a Static entry to MACT with a specific destination, it is 
 * expected that packets with this L2 address as their SA, will arrive from 
 * the source that was specified as the destination. 
 * When there is no match between the Source port/vlan and the Destination, 
 * transplant can be performed. 
 * The decision of learn/transplant can be used in the Field Processor. 
 * 
 * CINT Usage: 
 *  
 *  1. Run:
 *      cint cint_field_drop_static_sa_transplant.c
 *      cint
 *      field_drop_static_sa_transplant_example(int unit);
 *  
 *  2. In order to verify add static entry with some destination (port/vlan),
 *     and send packet with SA as L2 address in entry, from a different port.
 *     Packet should be dropped.
 */
int field_drop_static_sa_transplant_setup(/* in */ int unit,
                                          /* in */ int group_priority,
                                          /* out */ bcm_field_group_t *group)
{
    int grp;
    int result = 0;

    uint16 dq_offset = 42;
    uint8 dq_length = 1;
    uint8 data, mask;

    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_data_qualifier_t dq_is_dynamic; /* dynamic bit */
    bcm_field_qualify_t dq_qualifier = bcmFieldQualifyL2SrcValue;

    bcm_field_data_qualifier_t_init(&dq_is_dynamic); 
    dq_is_dynamic.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
    dq_is_dynamic.qualifier = dq_qualifier; 
    dq_is_dynamic.offset = dq_offset; 
    dq_is_dynamic.length = dq_length; 
    result = bcm_field_data_qualifier_create(unit, &dq_is_dynamic); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2StationMove);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcHit);

    result = bcm_field_qset_data_qualifier_add(unit, &qset, dq_is_dynamic.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }
    
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

    result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeL2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    result = bcm_field_qualify_L2StationMove(unit, ent, 1, 1);
    if (BCM_E_NONE != result) {
                printf("Error in bcm_field_qualify_L2StationMove\n");
        return result;
    }

    result = bcm_field_qualify_L2SrcHit(unit, ent, 0x1, 0x1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_L2SrcHit\n");
        return result;
    }
    
    data = 0x0;
    mask = 0x1;
    result = bcm_field_qualify_data(unit, ent, dq_is_dynamic.qual_id, &data, &mask, 1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data\n");
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
}

int field_drop_static_sa_transplant_example(/* in */ int unit)
{
    int result;
    int group_priority = 10;
    bcm_field_group_t group;

    result = field_drop_static_sa_transplant_setup(unit, group_priority, &group);
    if (BCM_E_NONE != result) {
        printf("Error in field_drop_static_sa_transplant_setup\n");
    }

    return result;
}
