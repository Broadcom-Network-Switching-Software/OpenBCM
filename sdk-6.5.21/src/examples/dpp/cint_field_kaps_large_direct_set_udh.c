/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_field_kaps_large_direct_set_udhc
 * Purpose: use pmf large direct to set the UDH header .
 * IPv6
 *  	set UDH0 with {Dest-VTEP-IPv4-Address[32b]}
 *    set UDH1 with {Reserved[4b], TOR-ID[28]}
 * IPv6
 *  	set UDH0 with {Dest-VTEP-IPv4-Address[32b]}
 *    set UDH1 with {Reserved[16b], IP-Cksum-Fix[16]}
 *
 * Usage:
 *    soc property pmf_kaps_large_db_size  to set the PMF large direct size.
 */
/********** 
  functions
 */
int field_kaps_large_direct_set_udh(int unit, int entry_num, uint32 *dest_vtep_info_ptr, uint32 *udh0, uint32 *udh1) {
    bcm_field_data_qualifier_t data_qual;
    int presel_id = 0;
    int presel_flags = 0;
    bcm_field_stage_t stage; 
    bcm_field_presel_set_t presel_set;    
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t action_entry;
    int result = 0;
    uint8 data[3], mask[3]; 
    int i;
    
 
    result = bcm_field_control_set(unit, bcmFieldControlLargeDirectLuKeyLength, 14);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_control_set with Err %x\n", result);
        return result;
    }
 
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.qualifier = bcmFieldQualifyL2DestValue;
    data_qual.length = 14;
    data_qual.offset = 20;
 
    result = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create with Err %x\n", result);
        return result;
    } 
    
    /*Define preselector*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            return result;
        } else {
            printf("bcm_field_presel_create_id: %x\n", presel_id);
        }
    } else {
        result = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            return result;
        } else {
            printf("bcm_field_presel_create_id: %x\n", presel_id);
        }
    }
    BCM_FIELD_PRESEL_INIT(presel_set);
    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);
 
    stage = bcmFieldStageIngress;
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
 
    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_LARGE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp.mode = bcmFieldGroupModeDirect;
    grp.group = 20; 
    grp.preselset = presel_set;
 
    /*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);
    
    result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (BCM_E_NONE != result) {
        printf("bcm_field_qset_data_qualifier_add %x\n", result);
        return result;
    }
    /* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n", result);
        return result;
    }
 
    BCM_FIELD_ASET_INIT(aset); 
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassSourceSet); 
 
    /* Attached the action to the field group */ 
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n", result);
        return result;
    }

    for (i = 0; i < entry_num; i++) {
        /*create an entry*/
        result = bcm_field_entry_create(unit, grp.group, &action_entry);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_entry_create Err %x\n", result);
            return result;
        }
        /* Fill the key and mask for the data qualifier. */
        data[0] = (dest_vtep_info_ptr[i] >> 16) & 0xFF;
        data[1] = (dest_vtep_info_ptr[i] >> 8) & 0xFF;
        data[2] = dest_vtep_info_ptr[i] & 0xFF;
        mask[0] = 0x00;
        mask[1] = 0x3F;
        mask[2] = 0xFF;
     
        result = bcm_field_qualify_data(unit, action_entry, data_qual.qual_id, &data, &mask, 3);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_data\n");
            return result;
        }
        result = bcm_field_action_add(unit, action_entry, bcmFieldActionClassDestSet, udh0[i], 0);
        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_action_add User-Header0:%x\n", udh0[i]);
            return result;
        }
     
        result = bcm_field_action_add(unit, action_entry, bcmFieldActionClassSourceSet, udh1[i], 0);
        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_action_add User-Header1:%x\n", udh1[i]);
            return result;
        }
    }
    
    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
       return result;
    }

    return result;
} 


/*
 * Following is for large direct with all four zone on QUX only
 *
 * EEI is locate on ZONE3 with 24 bits, set always valid to save 1 bit
 * Open SOC:  custom_feature_large_dir_without_valid=1
 */

bcm_field_presel_set_t psset_voip_v4;
int presel_id_voip_v4 = 0;
bcm_field_presel_set_t psset_voip_v6;
int presel_id_voip_v6 = 1;

uint32 classId = 0x37;
uint32 classIdmask = 0x7f;
uint32 msb = (classId & 0x20)>>5; /*separate the msb from the rest of the bits*/
uint32 lsbs = classId & 0x1F;     /*separate the 5 lsb bits*/
uint32 newClassId = ((msb<<19) | (1<<18) | (lsbs<<13));
int large_direct_lu_key_length=14;
int ipv6_fg = 0;
bcm_field_group_t group_1, group_2, group_3, group_4, group_5, group_6, group_7, group_8;

int pselect_setup(int unit, bcm_pbmp_t in_pbmp, int version)
{
    int result;
    int auxRes;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int presel_flags = 0;
    int presel_id = (version == 4 ? presel_id_voip_v4 : presel_id_voip_v6);

    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        printf("Advanced mode preselector\n");
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
            return result;
        }
    } else {
        printf("Basic mode preselector\n");
        result = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
            return result;
        }
    }

    /*
     * Set the preselector to be on the ports
     */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_ASSIGN(pbm, in_pbmp);
    for(i = 0; i < 256; i++)
    {
        BCM_PBMP_PORT_ADD(pbm_mask, i);
    }

    result = bcm_field_qualify_Stage(unit, presel_id | presel_flags | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_Stage\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
        return result;
    }

    result = bcm_field_qualify_InPorts(unit, presel_id | presel_flags | BCM_FIELD_QUALIFY_PRESEL, pbm, pbm_mask);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
        return result;
    }

    if (version == 4)
    {
        result = bcm_field_qualify_HeaderFormat(unit, presel_id | presel_flags | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp4AnyL2L3);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[0] in bcm_field_qualify_HeaderFormat\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
            return result;
        }
        BCM_FIELD_PRESEL_INIT(psset_voip_v4);
        BCM_FIELD_PRESEL_ADD(psset_voip_v4, presel_id);
    }
    else if (version == 6)
    {
        result = bcm_field_qualify_HeaderFormat(unit, presel_id | presel_flags | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp6AnyL2L3);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[0] in bcm_field_qualify_HeaderFormat\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
            return result;
        }
        BCM_FIELD_PRESEL_INIT(psset_voip_v6);
        BCM_FIELD_PRESEL_ADD(psset_voip_v6, presel_id);
    }
    else
    {
        result = BCM_E_ERROR;
        printf("Invalid IP version\n");
    }

    return result;
}

int pmf_stage1_lddb(int unit, bcm_field_presel_set_t preselector,
        /* in */ int group_priority_1,
        /* out */bcm_field_group_t *group_1,
        /* in */ int group_priority_2,
        /* out */bcm_field_group_t *group_2,
        int groupId,
        int start_index,
        int ecmp_id,
        int num_of_hosts,
        int *encap_id)
{
    bcm_field_group_config_t grp1;

    printf("pmf_stage1_lddb num_of_hosts %d\n",num_of_hosts);

    bcm_field_entry_t        db1_entry[num_of_hosts];
    int                      result = 0;
    int i;

    /*================================ First Group in a cascade (lookup in Large Direct DB) =================================*/
    printf("First Group in a cascade\n");
    int num_quals = 2;
    int qual_idx =0;
    if (ipv6_fg) { num_quals = 3; }

    bcm_field_data_qualifier_t  data_qual[num_quals];
    bcm_field_qualify_t key_dq_qualifier[num_quals];
    uint32 key_dq_length[num_quals];
    uint16 key_dq_offset[num_quals];


    if (ipv6_fg) {
        key_dq_qualifier[0] = bcmFieldQualifyConstantZero;
        key_dq_length[0] = 1;
        key_dq_offset[0] = 0;

        key_dq_qualifier[1] = bcmFieldQualifyConstantOne;
        key_dq_length[1] = 1;
        key_dq_offset[1] = 0;

        key_dq_qualifier[2] = bcmFieldQualifyConstantZero;
        key_dq_length[2] = large_direct_lu_key_length - 2;
        key_dq_offset[2] = 0;
    } else {
        key_dq_qualifier[0] = bcmFieldQualifyConstantZero;
        key_dq_length[0] = 2;
        key_dq_offset[0] = 0;

        key_dq_qualifier[1] = bcmFieldQualifyConstantZero;
        key_dq_length[1] = large_direct_lu_key_length - 2;
        key_dq_offset[1] = 0;
    }

    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp1);

    grp1.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_LARGE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp1.mode = bcmFieldGroupModeDirect;
    grp1.group = groupId;
    grp1.preselset = preselector;
    grp1.priority = group_priority_1;

    /* ---------------------- Configure DB -----------------------*/
    printf("Configure DB\n");
    /* Add qualifier set */
    BCM_FIELD_QSET_INIT(grp1.qset);

    /* Create new qualifier */

    for (qual_idx=0; qual_idx < num_quals; qual_idx++)
    {
        bcm_field_data_qualifier_t_init(&data_qual[qual_idx]);
        data_qual[qual_idx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;

        data_qual[qual_idx].length = key_dq_length[qual_idx];
        data_qual[qual_idx].offset = key_dq_offset[qual_idx];
        data_qual[qual_idx].qualifier = key_dq_qualifier[qual_idx];
        data_qual[qual_idx].stage = bcmFieldStageIngress;
        result = bcm_field_data_qualifier_create(unit, &data_qual[qual_idx]);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_data_qualifier_create\n");
            return result;
        }

        /* Add new qualifier to qualifier set */
        result = bcm_field_qset_data_qualifier_add(unit, &grp1.qset, data_qual[qual_idx].qual_id);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_qset_data_qualifier_add\n");
            return result;
        }
    }

    /* Create field group */
    result = bcm_field_group_config_create(unit, &grp1);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }

    /* Set Action set */
    BCM_FIELD_ASET_INIT(grp1.aset);
    BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionSystemHeaderSet);
    BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionCascadedKeyValueSet);

    /* Attach the Action set to the Field group */
    result = bcm_field_group_action_set(unit, grp1.group, grp1.aset);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }

    /* ---------------------------------- Add entries to Large Direct DB ----------------------------------*/
    /* ---------------------- a 128bit entry per host-----------------------*/
    printf("Add entries to DB num_of_hosts %d\n", num_of_hosts);
    int index = start_index;
    for (i=0; i<num_of_hosts; i++)
    {
        /* Create an entry */
        result = bcm_field_entry_create(unit, grp1.group, db1_entry[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_entry_create\n");
            return result;
        }

        /* Set a qualifier part of the entry */

        int byte;
        int kaps_mask = (1 << (large_direct_lu_key_length-2)) - 1;
        int size = 2; /*((large_direct_lu_key_length - 1) / 8) + 1;*/
        uint8 data1=0, mask1=3;
        uint8 data0=0, mask0=1;
        uint8 data2[2], mask2[2];

        if (ipv6_fg) {
            data0=0, mask0=1;
            data1=1, mask1=1;

            result = bcm_field_qualify_data(unit, db1_entry[i], data_qual[0].qual_id, &data0, &mask0, 1);
            if (BCM_E_NONE != result)
            {
                printf("Error in bcm_field_qualify_data\n");
                return result;
            }

            result = bcm_field_qualify_data(unit, db1_entry[i], data_qual[1].qual_id, &data1, &mask1, 1);
            if (BCM_E_NONE != result)
            {
                printf("Error in bcm_field_qualify_data\n");
                return result;
            }
    
            for (byte = 0; byte < size; byte++)
            {
                data2[byte] = (index >> ((size - byte - 1) * 8)) % 256;
                mask2[byte] = (kaps_mask >> ((size - byte - 1) * 8)) % 256;
            }
            printf("1: data1[0] = %x data1[1] = %x\n",data2[0], data2[1]);
            printf("1: mask[0] = %x mask[1] = %x\n",mask2[0], mask2[1]);
            result = bcm_field_qualify_data(unit, db1_entry[i], data_qual[2].qual_id, &data2, &mask2, 2);
            if (BCM_E_NONE != result)
            {
                printf("Error in bcm_field_qualify_data\n");
                return result;
            }
        } else {
            result = bcm_field_qualify_data(unit, db1_entry[i], data_qual[0].qual_id, &data1, &mask1, 1);
            if (BCM_E_NONE != result)
            {
                printf("Error in bcm_field_qualify_data\n");
                return result;
            }
    
            for (byte = 0; byte < size; byte++)
            {
                data2[byte] = (index >> ((size - byte - 1) * 8)) % 256;
                mask2[byte] = (kaps_mask >> ((size - byte - 1) * 8)) % 256;
            }
            printf("1: data1[0] = %x data1[1] = %x\n",data2[0], data2[1]);
            printf("1: mask[0] = %x mask[1] = %x\n",mask2[0], mask2[1]);

            result = bcm_field_qualify_data(unit, db1_entry[i], data_qual[1].qual_id, &data2, &mask2, 2);
            if (BCM_E_NONE != result)
            {
                printf("Error in bcm_field_qualify_data\n");
                return result;
            }
        }

        printf(">>>bcmFieldSystemHeaderPphEei 0x%x\n",encap_id[i]);
        result = bcm_field_action_add(unit, db1_entry[i], bcmFieldActionSystemHeaderSet,
                                      bcmFieldSystemHeaderPphEei /* Modify the Eei system header */, encap_id[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_action_add bcmFieldActionSystemHeaderSet\n");
            return result;
        }
        bcm_gport_t fec_dest;

        BCM_GPORT_FORWARD_PORT_SET(fec_dest, ecmp_id);

        printf("******** Add redirect ecmp %d, dst 0x%x\n",ecmp_id, fec_dest);
        result = bcm_field_action_add(unit, db1_entry[i], bcmFieldActionRedirect, 0, fec_dest);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_action_add bcmFieldActionRedirect\n");
            return result;
        }
        result = bcm_field_action_add(unit, db1_entry[i], bcmFieldActionCascadedKeyValueSet, 0x89, 0);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_action_add bcmFieldActionCascadedKeyValueSet\n");
            return result;
        }		
    }
    *group_1 = grp1.group;
    result = bcm_field_group_install(unit, grp1.group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install\n");
        return result;
    }

    return result;
}


int pmf_stage2_tcam(int unit, bcm_field_presel_set_t preselector, int group_id, bcm_field_group_t *group7, bcm_field_group_t *group8, int group_priority, int isIpv4)
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_config_t grp;
    bcm_field_entry_t        entry;
    bcm_field_group_config_t grp2;

    /*============================== Second Group in a cascade (lookup in TCAM) ===============================*/
    /* The second group in a cascade must be created first */
    printf("Second Group in a cascade, ecmp id\n");

    bcm_field_group_config_t_init(&grp);
    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp.group = group_id;
    grp.mode = bcmFieldGroupModeAuto;
    grp.preselset = preselector;

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyCascadedKeyValue);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);


    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    *group7 = grp.group;
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set, destroy the group...\n");
        rv = bcm_field_group_destroy(unit, grp.group);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_destroy\n");
            return rv;
        }
    }


    printf("Add entries to Filter DB %d\n", grp.group);
    rv = bcm_field_entry_create(unit, grp.group, &entry);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_entry_create 0\n");
        return rv;
    }


    rv = bcm_field_qualify_CascadedKeyValue(unit, entry, 0x88, 0xFF);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_qualify_CascadedKeyValue\n");
        return rv;
    }

    rv = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_action_add\n");
        return rc;
    }

    rv = bcm_field_entry_install(unit, entry);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_entry_install\n");
        return rv;
    }
    /*==========================Another TCAM Group===============================*/
    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp2);
    grp2.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_CYCLE;
    grp2.mode = bcmFieldGroupModeAuto;
    grp2.group = group_id+1;
    grp2.preselset = preselector;
    grp2.priority = group_priority+10;
    grp2.cycle = 1;

    /* ---------------------- Configure DB -----------------------*/
    printf("Configure DB\n");

    BCM_FIELD_QSET_INIT(grp2.qset);
    BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyL3DestRouteValue);
    BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyL4SrcPort);
    if (isIpv4 == 1)
    {
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyIpProtocol);
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyTos);
    }
    else
    {
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyIp6);
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyIp6NextHeader);
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyIp6TrafficClass);
    }

    BCM_FIELD_ASET_INIT(grp2.aset);

    BCM_FIELD_ASET_ADD(grp2.aset, bcmFieldActionMultipathHashAdditionalInfo);
    BCM_FIELD_ASET_ADD(grp2.aset, bcmFieldActionDscpNew);

    rv = bcm_field_group_config_create(unit, &grp2);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp2.group, grp2.aset);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_group_action_set\n");
        return rv;
    }


    /* ---------------------- Add entries to TCAM DB -----------------------*/
    bcm_field_entry_t        db2_entry[2];
    printf("Add entries to DB\n");
    /*entry#0*/
    rv = bcm_field_entry_create(unit, grp2.group, &db2_entry[0]);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_entry_create 0\n");
        return rv;
    }

    uint64 data;
    uint64 mask;


    printf("encoded class Id 0x%x\n", newClassId);
    COMPILER_64_SET(data, 0x00000000, newClassId);
    COMPILER_64_SET(mask, 0x00000000, classIdmask<<13);

    rv = bcm_field_qualify_L3DestRouteValue(unit, db2_entry[0], data, mask);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_petra_field_qualify_DstClassField\n");
        return rv;
    }
    if (isIpv4 == 1)
    {
        rv = bcm_field_qualify_IpProtocol(unit, db2_entry[0], 0x11, 0xff);
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_IpProtocol\n");
            return rv;
        }
    }
    else
    {
        rv = bcm_field_qualify_Ip6NextHeader(unit, db2_entry[0], 0x11, 0xff);
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_Ip6NextHeader\n");
            return rv;
        }
    }
    bcm_l4_port_t l4_src_port = 0x13C4;
    rv = bcm_field_qualify_L4SrcPort(unit, db2_entry[0], l4_src_port, 0xffff);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_qualify_L4SrcPort\n");
        return rv;
    }


    rv = bcm_field_action_add(unit, db2_entry[0], bcmFieldActionMultipathHashAdditionalInfo, 0, 0); /*offset 0*/
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_action_add\n");
        return rv;
    }

    rv = bcm_field_action_add(unit, db2_entry[0], bcmFieldActionDscpNew, 0xb9, 0);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_action_bcmFieldActionDscpNew_add\n");
        return rv;
    }

    /*entry#1*/
    rv = bcm_field_entry_create(unit, grp2.group, &db2_entry[1]);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    COMPILER_64_SET(data, 0x00000000, newClassId);
    COMPILER_64_SET(mask, 0x00000000, classIdmask<<13);

    rv = bcm_field_qualify_L3DestRouteValue(unit, db2_entry[1], data, mask);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_petra_field_qualify_DstClassField\n");
        return rv;
    }

    if (isIpv4 == 1)
    {
        rv = bcm_field_qualify_IpProtocol(unit, db2_entry[1], 0x11, 0xff);
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_IpProtocol\n");
            return rv;
        }
    }
    else
    {
        rv = bcm_field_qualify_Ip6NextHeader(unit, db2_entry[1], 0x11, 0xff);
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_Ip6NextHeader\n");
            return rv;
        }
    }

    rv = bcm_field_qualify_Tos(unit, db2_entry[1], 0xb8, 0xff);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_qualify_Tos\n");
        return rv;
    }


    rv = bcm_field_action_add(unit, db2_entry[1], bcmFieldActionMultipathHashAdditionalInfo, 0x8000, 0); /*offset 1*/
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_action_add\n");
        return rv;
    }


    rv = bcm_field_action_add(unit, db2_entry[1], bcmFieldActionDscpNew, 0xba, 0);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_action_bcmFieldActionDscpNew_add\n");
        return rv;
    }


    rv = bcm_field_group_install(unit, grp2.group);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }
    *group8 = grp2.group;


    return rv;
}


int update_dest_in_downstream(/* in */ int unit,
        /* in */ int port,
        /* in */ int group_priority_1,
        /* out */bcm_field_group_t *group_1,
        /* in */ int group_priority_2,
        /* out */bcm_field_group_t *group_2,
        /* in */ int groupId,
        /* in */ int num_of_hosts,
        /* in */ int *encap_id,
        /* in */ int ecmp_id,
        /* in */ int start_index)
{
    bcm_pbmp_t in_pbmp;

    BCM_PBMP_CLEAR(in_pbmp);
    BCM_PBMP_PORT_ADD(in_pbmp, port);

    /* Setup IPv4 preselector */

    int result = pselect_setup(unit, in_pbmp, 4);
    if (BCM_E_NONE != result) {
        printf("Error in pon_voip_pselect_setup result:%d\n", result);
        return result;
    }

    result = pmf_stage2_tcam(unit, psset_voip_v4, groupId+2, &group_5, &group_6, 23, 1);
    if (BCM_E_NONE != result) {
        printf("Error in pmf_stage2_tcam result:%d\n", result);
        return result;
    }


    result = pmf_stage1_lddb(unit, psset_voip_v4, group_priority_1, group_1, group_priority_2, group_2, groupId, start_index, ecmp_id, num_of_hosts, encap_id);

    if (BCM_E_NONE != result) {
        printf("Error in pmf_stage1_lddb result:%d\n", result);
        return result;
    }
    return result;
}


int update_dest_in_downstream_ipv6(/* in */ int unit,
        /* in */ int port,
        /* in */ int group_priority_1,
        /* out */bcm_field_group_t *group_1,
        /* in */ int group_priority_2,
        /* out */bcm_field_group_t *group_2,
        /* in  */int groupId,
        /* in */ int num_of_hosts,
        /* in */ int *encap_id,
        /* in */ int ecmp_id,
        /* in */ int start_index)
{
    bcm_pbmp_t in_pbmp;
    BCM_PBMP_CLEAR(in_pbmp);
    BCM_PBMP_PORT_ADD(in_pbmp, port);

    /* Setup IPv6 preselector */
    int result = pselect_setup(unit, in_pbmp, 6);
    if (BCM_E_NONE != result) {
        printf("Error in pon_voip_pselect_setup result:%d\n", result);
        return result;
    } else {
        printf("IPv6 preselector created\n");
    }

    result = pmf_stage2_tcam(unit, psset_voip_v6, groupId, &group_7, &group_8, 22, 0);
    if (BCM_E_NONE != result) {
        printf("Error in pmf_stage2_tcam result:%d\n", result);
        return result;
    }

    result = pmf_stage1_lddb(unit, psset_voip_v6, group_priority_1, group_1, group_priority_2, group_2, groupId+2 ,start_index, ecmp_id, num_of_hosts, encap_id);
    if (BCM_E_NONE != result) {
        printf("Error in pmf_stage1_lddb result:%d\n", result);
        return result;
    }

    return result;
}

int main_kaps_service(int unit, int src_port)
{
    int rv=BCM_E_NONE;
    int encap_id[2];
    
    rv = bcm_field_control_set(unit, bcmFieldControlLargeDirectLuKeyLength, large_direct_lu_key_length);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_control_set for bcmFieldControlLargeDirectLuKeyLength, result:%d\n", rv);
        return rv;
    }

    rv = bcm_field_control_set(unit, bcmFieldControlCascadedKeyWidth, 8);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_control_set for bcmFieldControlCascadedKeyWidth, result:%d\n", rv);
        return rv;
    }

    
    encap_id[0] = 0xc02774;
    rv = update_dest_in_downstream(unit, src_port, 3, &group_3, 5, &group_4, 17, 1, &encap_id[0], 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_control_set for bcmFieldControlCascadedKeyWidth, result:%d\n", rv);
        return rv;
    }

    ipv6_fg = 1;
    encap_id[0] = 0xc02775;
    rv = update_dest_in_downstream_ipv6(unit, src_port, 2, &group_1, 4, &group_2, 10, 1, &encap_id[0], 2, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_control_set for bcmFieldControlCascadedKeyWidth, result:%d\n", rv);
        return rv;
    }

    return rv;
}


