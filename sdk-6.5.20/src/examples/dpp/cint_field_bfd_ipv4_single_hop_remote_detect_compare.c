/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 

int snoop_id;
int snoop_trap_id;

bcm_field_group_config_t grp_a, grp_b;
bcm_field_entry_t ent_a, ent_b;

/**
 * Snoop creating:
 *
 * @param unit
 * @param dest_port
 *
 * @return snoop id
 */
int snoop_create(int unit, bcm_gport_t dest, int* id)
{   
    int rv = BCM_E_NONE;
    bcm_rx_snoop_config_t snoop_config;
    int snoop_id;

    /* create */
    rv = bcm_rx_snoop_create(unit, 0, &snoop_id);
    printf("created snoop %d \n", snoop_id);
    *id = snoop_id;
    
    if (rv == BCM_E_RESOURCE) {
        printf("snoop already created\n");
        return rv;
    }
    else if (rv != BCM_E_NONE) {
        printf("Error in: bcm_rx_snoop_create rv %d\n", rv);
        return rv;
    }      
    
    /* configure trap attribute to update destination */
    sal_memset(&snoop_config, 0, sizeof(snoop_config));
    snoop_config.flags = (BCM_RX_SNOOP_UPDATE_DEST);
    snoop_config.probability = 100000;
    snoop_config.dest_port = dest;
    snoop_config.size = -1;

    rv = bcm_rx_snoop_set(unit, snoop_id, &snoop_config);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_snoop_set returned %d\n", rv);
        return rv;
    }
    
    return rv;
}

/**
 * Create trap for snoop:
 *
 * @param unit
 * @param snoop type
 * @param snoop comand
 * @param snoop strength
 *
 * @return trap id
 */
int trap_for_snoop(int unit, int type, int snoop_cmd, int snoop_strength, int* trapid)
{   
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    int trap_id;

    /* create trap */
    rv = bcm_rx_trap_type_create(unit, 0, type, &trap_id);
    printf("created trap %d \n", trap_id);
    *trapid = trap_id;
    
    if (rv == BCM_E_RESOURCE) {
        printf("trap already created\n");
        return rv;
    }
    else if (rv != BCM_E_NONE) {
        printf("Error in: bcm_rx_trap_type_create rv %d\n", rv);
        return rv;
    }      
    
    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = 0;
    trap_config.trap_strength = 0;
    trap_config.snoop_cmnd = snoop_cmd;
    trap_config.snoop_strength = snoop_strength;
    
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_set returned %d\n", rv);
        return rv;
    }
    
    return rv;
}


int bfd_ipv4_single_hop_compare_fg_set(int unit, bcm_field_presel_set_t presel_set)
{
    int result;
    int group_priority_a = 50;
    int group_priority_b = 60;

    result = snoop_create(0, 0, &snoop_id);
    if (BCM_E_NONE != result) {
        printf("Error in snoop_create\n");
        return result;
    }

    result = trap_for_snoop(0, bcmRxTrapUserDefine, snoop_id, 3, &snoop_trap_id);
    if (BCM_E_NONE != result) {
        printf("Error in trap_for_snoop\n");
        return result;
    }

    /*
     * Next two functions bfd_compare_fg_set_* creating two fields group for compare operation mode.
     * We are comparing the Detect Multiplier from the packet vs. Detect Multiplier from LEM
     * In case the values are different packet is snooped according snoop configured above.
     */
    result = bfd_compare_fg_set_a(unit,
                                  presel_set,
                                  group_priority_a);
    if (BCM_E_NONE != result) {
        printf("Error in bfd_compare_fg_set_a\n");
        return result;
    }

    result = bfd_compare_fg_set_b(unit,
                                  presel_set,
                                  group_priority_b);
    if (BCM_E_NONE != result) {
        printf("Error in bfd_compare_fg_set_b\n");
        return result;
    }

    return result;

}

/*
 * Configuring first group for comparing(detect multiplier from packet).
 */
int bfd_compare_fg_set_a(/* in */  int unit,
                         /* in */  bcm_field_presel_set_t psset,
                         /* in */  int group_priority)
{
    int result;
    bcm_field_aset_t aset;
    bcm_field_data_qualifier_t data_qual_pkt;
    bcm_field_data_qualifier_t data_qual_zero;

    /*
     * Define the first Field Group
     */
    bcm_field_group_config_t_init(&grp_a);
    grp_a.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp_a.flags |= BCM_FIELD_GROUP_CREATE_IS_EQUAL;
    grp_a.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp_a.mode = bcmFieldGroupModeAuto;

    grp_a.priority = group_priority;
    grp_a.preselset = psset;

    /*
     * Define the QSET.
     * No need to use IsEqual as qualifier for this field group.
     */
    BCM_FIELD_QSET_INIT(grp_a.qset);
    BCM_FIELD_QSET_ADD(grp_a.qset, bcmFieldQualifyStageIngress);


    /* 
     * Select remote detect multiplier from the BFD packet via a data qualifier
     */
    bcm_field_data_qualifier_t_init(&data_qual_pkt);
    data_qual_pkt.offset_base = bcmFieldDataOffsetBaseForwardingHeader;
    data_qual_pkt.offset = 30; /* offset 30 bytes(Detect Multiupliere 20B(IP) + 8B(UDP) + 1B(Version_Diag - BFD) + 1B(Flags BFD)from L3 hdr*/
    data_qual_pkt.length = 1; 
    data_qual_pkt.stage = bcmFieldStageIngress;
    result = bcm_field_data_qualifier_create(unit, &data_qual_pkt);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp_a.qset, data_qual_pkt.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }
    
    /* 
     * Pad with 12 bits of zero to get to 20 bits
     */
    bcm_field_data_qualifier_t_init(&data_qual_zero);
    data_qual_zero.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual_zero.offset = 0;
    data_qual_zero.qualifier = bcmFieldQualifyConstantZero;
    data_qual_zero.length = 12;
    data_qual_zero.stage = bcmFieldStageIngress;
    result = bcm_field_data_qualifier_create(unit, &data_qual_zero);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }
    
    result = bcm_field_qset_data_qualifier_add(unit, &grp_a.qset, data_qual_zero.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }

    /*
     *  Define the ASET 
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew); /* dummy action */

    /*  Create the Field group */
    result = bcm_field_group_config_create(unit, &grp_a);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create for grp_a.group %d\n", grp_a.group);
        return result;
    }

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp_a.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set for group %d\n", grp_a.group);
        return result;
    }

    /* create an entry and install it */
    result = bcm_field_entry_create(unit, grp_a.group, &ent_a);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    result = bcm_field_entry_install(unit, ent_a);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    printf("\n\n**** (A) bfd_compare_fg_set_a for group %d Complete!!\n\n", grp_a.group);

    return result;

} /* compare_field_group_set_a */


/*
 * Configuring second group for comparing(detect multiplier from LEM).
 */
int bfd_compare_fg_set_b(/* in */  int unit,
                         /* in */  bcm_field_presel_set_t psset,
                         /* in */  int group_priority)
{
    int result;
    int dq_idx;
    uint8 data_mask;
    bcm_field_aset_t aset;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t is_equal[2];

    bcm_field_qualify_t qual = bcmFieldQualifyL2DestValue;   
                                                             
    bcm_field_data_qualifier_t data_qual_detect_1;
    bcm_field_data_qualifier_t data_qual_detect_2;
    bcm_field_data_qualifier_t data_qual_zero;

    /*
     * Define the second Field Group (MSB bits of key 7).
     */
    bcm_field_group_config_t_init(&grp_b);
    grp_b.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp_b.flags |= BCM_FIELD_GROUP_CREATE_IS_EQUAL;
    grp_b.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp_b.mode = bcmFieldGroupModeDirectExtraction;
    grp_b.priority = group_priority;
    grp_b.preselset = psset;

    /*
     * Define the QSET
     */
    BCM_FIELD_QSET_INIT(grp_b.qset);
    BCM_FIELD_QSET_ADD(grp_b.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp_b.qset, bcmFieldQualifyIsEqualValue);

    /*
     *  Define the ASET.
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);

    /*
     * the expected remote detection mult sits at bit 16 of LEM result
     */
    bcm_field_data_qualifier_t_init(&data_qual_detect_1);
    data_qual_detect_1.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual_detect_1.offset = 16;
    data_qual_detect_1.length = 8;
    data_qual_detect_1.qualifier = qual;
    result = bcm_field_data_qualifier_create(unit, &data_qual_detect_1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create - data_qual Err %x\n",result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp_b.qset, data_qual_detect_1.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }
    
    /* 
     * Pad with 12 bits of zero to get to 20 bits 
     */
    bcm_field_data_qualifier_t_init(&data_qual_zero);
    data_qual_zero.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual_zero.offset = 0;
    data_qual_zero.qualifier = bcmFieldQualifyConstantZero;
    data_qual_zero.length = 12;
    data_qual_zero.stage = bcmFieldStageIngress;
    result = bcm_field_data_qualifier_create(unit, &data_qual_zero);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }
    
    result = bcm_field_qset_data_qualifier_add(unit, &grp_b.qset, data_qual_zero.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }

    /*  Create the Field group with type Direct Extraction */
    result = bcm_field_group_config_create(unit, &grp_b);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create for group %d\n", grp_b.group);
        return result;
    }

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp_b.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set for group %d\n", grp_b.group);
        return result;
    }

    /*
     * Create the Direct Extraction entry:
     *  1. create the entry
     *  2. Construct the action:
     *     bit 0 = action valid
     *     bits 3:0 = IsEqual  (4 bits, each indicating equality of 20-bit chunks)
     *  3. Install entry (HW configuration)
     */
    result = bcm_field_entry_create(unit, grp_b.group, &ent_b);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    /*
     * IsEqualValue Qualifier:
     *    IsEqualValue qualifier has 4 bits, each indicated
     *    match of 20 bits in the compare key. The LSB bit
     *    corresponds to the LSB 20 compared bits, and so on.
     */
    result = bcm_field_qualify_IsEqualValue(unit, ent_b, 0x0, 0x1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_IsEqualValue\n");
        return result;
    }

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&is_equal[0]);
    bcm_field_extraction_field_t_init(&is_equal[1]);

    extract.action = bcmFieldActionSnoop;  /* action for comparison result == 0 */
    extract.bias = 0;

    is_equal[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    is_equal[0].bits  = 8;
    is_equal[0].lsb = 0;
    is_equal[0].value = snoop_trap_id;

    is_equal[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    is_equal[1].bits  = 2;
    is_equal[1].lsb = 0;
    is_equal[1].value = 3; /* snoop strength */

    result = bcm_field_direct_extraction_action_add(unit,
                                                    ent_b,
                                                    extract,
                                                    2 /* count */,
                                                    &is_equal);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    /* Write entry to HW */
    result = bcm_field_entry_install(unit, ent_b);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    printf("\n\n**** (B) bfd_compare_fg_set_b for group %d Complete!!\n\n", grp_b.group);

    return result;

} /* compare_field_group_set_b */
