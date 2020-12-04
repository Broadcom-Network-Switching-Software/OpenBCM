/* $Id: cint_field_fwd_ipv6_uc_with_rpf_2pass.c,v 1.0
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * File: cint_field_fwd_ipv6_uc_with_rpf_2pass.c
 * Purpose: Example of SETTING PMF actions for IPv6 UC with RPF in 2 PASS feature.
 * Note: This feature realy on using KBP (ELK) device for FLP FWD TCAM 
 *  
 *  Cint code to define the following 2 PMF programs:
 *  1. Cint code for changing Packet Forwarding code to IPv4 to enable
 *     RPF Strick check in FEC block.
 *     This program is selected when FLP program selected is the IPv6_UC_2PASS (in the first packet cycle)
 *  2. Cint code to trigger IPv6_UC_RPF_Loose_Failure_TRAP,
 *     to overcome FLP block issue limit for IPv4 loose failure only
 *  
 *  main function: field_fwd_ipv6UC_uc_with_rpf_2pass_init_default calling both PMF defintion above.
 *  Sub Functions:
 *  1. field_fwd_ipv6UC_uc_loose_rpf_2pass(int unit, int * field_presel_id):
 *     Create field group with program selection IPv6 and No hit in KBP ; TCAM RPF lookup.
 *     Using Direct extraction.
 *     Action - TRAP bcmRxTrapUcLooseRpfFail
 *  2. field_fwd_ipv6UC_uc_with_rpf_2pass(int unit, int * field_presel_id):
 *     Create field group with program selection IPv6
 *     Using Direct extraction.
 *     Action: Change Forward-code to IPv4 for FEC block will perfrom RPF Strict Validation
 *  NOTE: It is clear that the field group priority define for trigger bcmRxTrapUcLooseRpfFail TRAP
 *  must be higher then the FWD Strict validation.
 *  
 *  General Sequence:
 *  1. 	Define a preselection with FLP Profile as qualifier.
 *      Only packets which handled by the the FLP program from FLP-IPv6UC-RPF_2PASS
 *      will be handled.  
 *  2. 	Create a Field Group and add an entry.
 *  3.  Create direct extarction action to overwride packet Forward-Code to IPv4 ; Or bcmRxTrapUcLooseRpfFail TRAP
 *  4. 	Create the connection between the Presel and Field Group.
 *  5.  Install Filed Group.
 *  
 *  calling Sequence:
 *  cint ../../../../src/examples/dpp/cint_field_fwd_ipv6_uc_with_rpf_2pass.c
 *  
 *  cint
 *  int unit = 0;
 *  
 *  print field_fwd_ipv6UC_uc_with_rpf_2pass_init_default(unit);
 *  exit;
 */

int field_fwd_ipv6UC_uc_with_rpf_2pass_init_default(int unit)
{
    int result = BCM_E_NONE;

    int strict_presel_id, loose_presel_id;
    
    result = field_fwd_ipv6UC_uc_loose_rpf_2pass(unit, &loose_presel_id);
    if (BCM_E_NONE != result) {
        printf("Error in field_fwd_ipv6UC_uc_loose_rpf_2pass\n");
        return result;
    }
    else {
        printf("field_fwd_ipv6UC_uc_loose_rpf_2pass: loose_presel_id %x\n", loose_presel_id);
    }

    result = field_fwd_ipv6UC_uc_with_rpf_2pass(unit, &strict_presel_id);
    if (BCM_E_NONE != result) {
        printf("Error in field_fwd_ipv6UC_uc_with_rpf_2pass\n");
        return result;
    }
    else {
        printf("field_fwd_ipv6UC_uc_with_rpf_2pass: strict_presel_id %x\n", strict_presel_id);
    }

    printf("field_fwd_ipv6UC_uc_with_rpf_2pass_init_default: COMPLETED\n");
    return result;
}

int field_fwd_ipv6UC_uc_loose_rpf_2pass(int unit, int * field_presel_id)
{
    int result = BCM_E_NONE;
    bcm_field_presel_set_t presel_set;  
    int presel_id = 0;
    int presel_flags = 0;

    bcm_field_group_config_t loose_conf_group;
    int group_id = 8; 
    int group_priority = 20;

    bcm_field_data_qualifier_t trap_code_qual_0, trap_code_qual_1, trap_code_qual_2, trap_code_qual_3, trap_code_qual_4, trap_code_qual_5;
    bcm_field_data_qualifier_t strength_qual;

	
    bcm_field_aset_t rpf_loose_aset;

    bcm_field_entry_t action_entry;

    bcm_field_stage_t stage;

    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
    } else {
        result = bcm_field_presel_create(unit, &presel_id);
    }

    if (BCM_E_NONE != result) {
        print("Error in ipv6UC-RPF-2PASS in loose rpf presel_id\n");
        result = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }
    else {
        printf("ipv6UC-RPF-2PASS loose rpf presel_id: %x\n", presel_id);
    }
	
	BCM_FIELD_PRESEL_INIT(presel_set);
    int internalFieldRpfLookupFailed = 0x0;

    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);

    /* Program Selection according to IPv6UCRPF 2 PASS */
    result = bcm_field_qualify_AppType(unit,presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldAppTypeIp6UcastRpf);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_AppType\n");
        result = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }

    /* Program Selection according whether there was ahit / or not on the external TCAM (KBP) */
    result = bcm_field_qualify_ExternalHit1(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, internalFieldRpfLookupFailed, 0x1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ExternalHit1\n");
        result = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }

    /* Attached the Pre-selector */
    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);

    { 
        /* Build field qualifier (lookup key) Structures
         * Note - we use in out filed group only direct extraction to raise RPF-Loose-Failed TRAP
         * For doing that we need to set the key with value 166 0xA6 (1010:0110), starting from lsb 
         */ 

        /* copy 1b'0 [0:0]lsb) */
        bcm_field_data_qualifier_t_init(&trap_code_qual_0); /* trap code qualifier - to use after in direct extraction */
        trap_code_qual_0.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        trap_code_qual_0.offset = 0; /* lsb of the key */
        trap_code_qual_0.length = 1; /* bits */
        trap_code_qual_0.qualifier = bcmFieldQualifyConstantZero;
        result = bcm_field_data_qualifier_create(unit, &trap_code_qual_0);
        if (BCM_E_NONE != result) {
            print bcm_field_show(unit,"");
            printf ("Error: bcm_field_data_qualifier_create trap_code_qual_0");
            return result;
        }

        /* copy 2b'1 [2:1] lsb) */
        bcm_field_data_qualifier_t_init(&trap_code_qual_1); /* trap code qualifier - to use after in direct extraction */
        trap_code_qual_1.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        trap_code_qual_1.offset = 0; /* lsb of the key */
        trap_code_qual_1.length = 2; /* bits */
        trap_code_qual_1.qualifier = bcmFieldQualifyConstantOne;
        result = bcm_field_data_qualifier_create(unit, &trap_code_qual_1);
        if (BCM_E_NONE != result) {
            print bcm_field_show(unit,"");
            printf ("Error: bcm_field_data_qualifier_create trap_code_qual_1");
            return result;
        }

        /* copy 2b'0 [4:3] lsb) */
        bcm_field_data_qualifier_t_init(&trap_code_qual_2); /* trap code qualifier - to use after in direct extraction */
        trap_code_qual_2.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        trap_code_qual_2.offset = 0; /* lsb of the key */
        trap_code_qual_2.length = 2; /* bits */
        trap_code_qual_2.qualifier = bcmFieldQualifyConstantZero;
        result = bcm_field_data_qualifier_create(unit, &trap_code_qual_2);
        if (BCM_E_NONE != result) {
            print bcm_field_show(unit,"");
            printf ("Error: bcm_field_data_qualifier_create trap_code_qual_2");
            return result;
        }

        /* copy 1b'1 [5:5] lsb) */
        bcm_field_data_qualifier_t_init(&trap_code_qual_3); /* trap code qualifier - to use after in direct extraction */
        trap_code_qual_3.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        trap_code_qual_3.offset = 0; /* lsb of the key */
        trap_code_qual_3.length = 1; /* bits */
        trap_code_qual_3.qualifier = bcmFieldQualifyConstantOne;
        result = bcm_field_data_qualifier_create(unit, &trap_code_qual_3);
        if (BCM_E_NONE != result) {
            print bcm_field_show(unit,"");
            printf ("Error: bcm_field_data_qualifier_create trap_code_qual_3");
            return result;
        }

        /* copy 1b'0 [6:6] lsb) */
        bcm_field_data_qualifier_t_init(&trap_code_qual_4); /* trap code qualifier - to use after in direct extraction */
        trap_code_qual_4.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        trap_code_qual_4.offset = 0; /* lsb of the key */
        trap_code_qual_4.length = 1; /* bits */
        trap_code_qual_4.qualifier = bcmFieldQualifyConstantZero;
        result = bcm_field_data_qualifier_create(unit, &trap_code_qual_4);
        if (BCM_E_NONE != result) {
            print bcm_field_show(unit,"");
            printf ("Error: bcm_field_data_qualifier_create trap_code_qual_4");
            return result;
        }

        /* copy 1b'1 [7:7] lsb) */
        bcm_field_data_qualifier_t_init(&trap_code_qual_5); /* trap code qualifier - to use after in direct extraction */
        trap_code_qual_5.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        trap_code_qual_5.offset = 0; /* lsb of the key */
        trap_code_qual_5.length = 1; /* bits */
        trap_code_qual_5.qualifier = bcmFieldQualifyConstantOne;
        result = bcm_field_data_qualifier_create(unit, &trap_code_qual_5);
        if (BCM_E_NONE != result) {
            print bcm_field_show(unit,"");
            printf ("Error: bcm_field_data_qualifier_create trap_code_qual_5");
            return result;
        }

        /* Add Trap Strength bits [9:10] to the key */
        bcm_field_data_qualifier_t_init(&strength_qual); /* Trap Strength */
        strength_qual.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        strength_qual.offset = 0; /* points to the lsb of key */
        strength_qual.length = 3; /* bits */
        strength_qual.qualifier = bcmFieldQualifyConstantOne;  /* all 1 == strength 7 */
        result = bcm_field_data_qualifier_create(unit, &strength_qual);
        if (BCM_E_NONE != result) {
            print bcm_field_show(unit,"");
            printf("Error: bcm_field_data_qualifier_create strength_qual");
            return result;
        }
    }

    /* Initialized Field Group */
    bcm_field_group_config_t_init(&loose_conf_group);

    /*
     * Define Programabble Field Group
     */
    loose_conf_group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    loose_conf_group.mode = bcmFieldGroupModeDirectExtraction;
    loose_conf_group.priority = group_priority;
    loose_conf_group.preselset = presel_set;
    {
        /* Associate the Qualifier with the field group qset */
        BCM_FIELD_QSET_INIT(loose_conf_group.qset);

        result = bcm_field_qset_data_qualifier_add(unit,
                                                 loose_conf_group.qset,
                                                 trap_code_qual_0.qual_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qset_data_qualifier_add trap_code_qual_0, Error %x\n",result);
            return result;
        }
        printf("trap_code_qual_0.qual_id, is %x\n",trap_code_qual_0.qual_id);

        result = bcm_field_qset_data_qualifier_add(unit,
                                                 loose_conf_group.qset,
                                                 trap_code_qual_1.qual_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qset_data_qualifier_add trap_code_qual_1, Error %x\n",result);
            return result;
        }
        printf("trap_code_qual_1.qual_id, is %x\n",trap_code_qual_1.qual_id);

        result = bcm_field_qset_data_qualifier_add(unit,
                                                 loose_conf_group.qset,
                                                 trap_code_qual_2.qual_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qset_data_qualifier_add trap_code_qual_2, Error %x\n",result);
            return result;
        }
        printf("trap_code_qual_2.qual_id, is %x\n",trap_code_qual_2.qual_id);

        result = bcm_field_qset_data_qualifier_add(unit,
                                                 loose_conf_group.qset,
                                                 trap_code_qual_3.qual_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qset_data_qualifier_add trap_code_qual_3, Error %x\n",result);
            return result;
        }
        printf("trap_code_qual_3.qual_id, is %x\n",trap_code_qual_3.qual_id);

        result = bcm_field_qset_data_qualifier_add(unit,
                                                 loose_conf_group.qset,
                                                 trap_code_qual_4.qual_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qset_data_qualifier_add trap_code_qual_4, Error %x\n",result);
            return result;
        }
        printf("trap_code_qual_4.qual_id, is %x\n",trap_code_qual_4.qual_id);

        result = bcm_field_qset_data_qualifier_add(unit,
                                                 loose_conf_group.qset,
                                                 trap_code_qual_5.qual_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qset_data_qualifier_add trap_code_qual_5, Error %x\n",result);
            return result;
        }
        printf("trap_code_qual_5.qual_id, is %x\n",trap_code_qual_5.qual_id);

        result = bcm_field_qset_data_qualifier_add(unit,
                                                 loose_conf_group.qset,
                                                 strength_qual.qual_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qset_data_qualifier_add strength_qual, Error %x\n",result);
            return result;
        }
        printf("strength_qual.qual_id, %x\n",strength_qual.qual_id);
    }

    /* create field group */
    result = bcm_field_group_config_create(unit, &loose_conf_group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create for loose_conf_group with Err %x\n",result);
        return result;
    }
    else {
        printf("bcm_field_group_config_create: with group id %x\n", loose_conf_group.group);
    }

    /* Initiate Field group Actions */
    BCM_FIELD_ASET_INIT(rpf_loose_aset);
    BCM_FIELD_ASET_ADD(rpf_loose_aset, bcmFieldActionTrap); /* create the actiontype to change to raise a trap */

     /* Attached the action type to the field group */
    result = bcm_field_group_action_set(unit, loose_conf_group.group, rpf_loose_aset);
    if (BCM_E_NONE != result) {
        printf("Error Attache aset in bcm_field_group_action_set Err %x\n",result);
        return result;
    }

    /*create field group Action Entry */
    result = bcm_field_entry_create(unit, loose_conf_group.group, &action_entry);
    if (BCM_E_NONE != result) {
        printf("Error in Create Entry calling bcm_field_entry_create Err %x\n",result);
        return result;
    }

    /* Define Action copy instructions */
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t trapType_action[7];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&trapType_action);

    extract.action = bcmFieldActionTrap;
    extract.bias = 0;

    {
        /* Copy the Trap code (HW Trap Code) bits [0:0] */
        trapType_action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        trapType_action[0].bits  = 1;
        trapType_action[0].lsb = 0;
        trapType_action[0].qualifier = trap_code_qual_0.qual_id;

        /* Copy the Trap code (HW Trap Code) bits [2:1] */
        trapType_action[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        trapType_action[1].bits  = 2;
        trapType_action[1].lsb = 0;
        trapType_action[1].qualifier = trap_code_qual_1.qual_id;

        /* Copy the Trap code (HW Trap Code) bits [4:3] */
        trapType_action[2].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        trapType_action[2].bits  = 2;
        trapType_action[2].lsb = 0;
        trapType_action[2].qualifier = trap_code_qual_2.qual_id;

        /* Copy the Trap code (HW Trap Code) bits [5] */
        trapType_action[3].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        trapType_action[3].bits  = 1;
        trapType_action[3].lsb = 0;
        trapType_action[3].qualifier = trap_code_qual_3.qual_id;

        /* Copy the Trap code (HW Trap Code) bits [6] */
        trapType_action[4].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        trapType_action[4].bits  = 1;
        trapType_action[4].lsb = 0;
        trapType_action[4].qualifier = trap_code_qual_4.qual_id;

        /* Copy the Trap code (HW Trap Code) bits [7] */
        trapType_action[5].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        trapType_action[5].bits  = 1;
        trapType_action[5].lsb = 0;
        trapType_action[5].qualifier = trap_code_qual_5.qual_id;

        /* Copy the Trap Strength */
        trapType_action[6].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        trapType_action[6].bits  = 3;
        trapType_action[6].lsb = 0;
        trapType_action[6].qualifier = strength_qual.qual_id;
    }

    result = bcm_field_direct_extraction_action_add(unit,action_entry,extract,7,&trapType_action);
    if (BCM_E_NONE != result) {
        printf("Error calling bcm_field_direct_extraction_action_add Err %x\n",result);
        return result;
    }
    else {
        printf("Success calling bcm_field_direct_extraction_action_add action %x\n",action_entry);
    }

    result = bcm_field_group_install(unit, loose_conf_group.group);	
    if (BCM_E_NONE != result) {
        printf("Error calling bcm_field_group_install loose_conf_group Err %x\n",result);
        return result;
    }
    else
    {
        printf("Success calling bcm_field_group_install group %x\n",loose_conf_group.group);
    }

    *field_presel_id = presel_id;

    printf("field_fwd_ipv6UC_uc_loose_rpf_2pass: Done Field Group Setting Loose RPF Trap\n");
    bcm_field_group_dump(unit,loose_conf_group.group);

    return result;
}


int field_fwd_ipv6UC_uc_with_rpf_2pass(int unit, int * field_presel_id)
{
    int result = BCM_E_NONE;
    bcm_field_presel_set_t presel_set;  
    int presel_id = 1;
    int presel_flags = 0;

    bcm_field_group_config_t fwd_code_group;
    int group_id = 10; 
    int group_priority = 12;
	
    bcm_field_aset_t fwd_code_aset;

    bcm_field_entry_t action_entry;

    bcm_field_stage_t stage;

    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
    } else {
        result = bcm_field_presel_create(unit, &presel_id);
    }
    if (BCM_E_NONE != result) {
        printf("Error in ipv6UC-RPF-2PASS in fwd to FEC strict rpf presel_id\n");
        result = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }
    else {
        printf("ipv6UC-RPF-2PASS in fwd to FEC strict rpf presel_id: %x\n", presel_id);
    }

    BCM_FIELD_PRESEL_INIT(presel_set);

    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    result = bcm_field_qualify_AppType(unit,presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldAppTypeIp6UcastRpf);

    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);

    BCM_FIELD_ASET_INIT(fwd_code_aset);
    BCM_FIELD_ASET_ADD(fwd_code_aset, bcmFieldActionForwardingTypeNew); /* create the actiontype to change the Forwarding Code */

    /* Initialized Field Group */
    bcm_field_group_config_t_init(&fwd_code_group);

    /*
     * Define Programabble Field Group
     */
    /* fwd_code_group.group = group; */
    fwd_code_group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
                           /* BCM_FIELD_GROUP_CREATE_WITH_ID; */
    fwd_code_group.mode = bcmFieldGroupModeDirectExtraction;
    fwd_code_group.priority = group_priority;
    fwd_code_group.preselset = presel_set;

    BCM_FIELD_QSET_INIT(fwd_code_group.qset);
    BCM_FIELD_QSET_ADD(fwd_code_group.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(fwd_code_group.qset, bcmFieldQualifyConstantZero);

    result = bcm_field_group_config_create(unit, &fwd_code_group);
    if (BCM_E_NONE != result) {
        printf("Error in field_fwd_ipv6UC_uc_with_rpf_2pass calling bcm_field_group_config_create with Err %x\n",result);
        return result;
    }
    else {
        printf("field_fwd_ipv6UC_uc_with_rpf_2pass OK bcm_field_group_config_create: %x\n", result);
        group_id = fwd_code_group.group;
        printf("field_fwd_ipv6UC_uc_with_rpf_2pass group_id: %x\n", group_id);
    }

     /* Attached the action type to the field group */
    result = bcm_field_group_action_set(unit, fwd_code_group.group, fwd_code_aset);
    if (BCM_E_NONE != result) {
        printf("Error in field_fwd_ipv6UC_uc_with_rpf_2pass calling bcm_field_group_action_set Err %x\n",result);
        return result;
    }

    /*create an entry*/
    result = bcm_field_entry_create(unit, fwd_code_group.group, &action_entry);
    if (BCM_E_NONE != result) {
        printf("Error in field_fwd_ipv6UC_uc_with_rpf_2pass calling bcm_field_entry_create Err %x\n",result);
        return result;
    }

    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t newAppType_action;

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&newAppType_action);

    extract.action = bcmFieldActionForwardingTypeNew;
    extract.bias = 1;

    newAppType_action.value = 0;
    newAppType_action.bits = 4;  
    newAppType_action.flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    newAppType_action.lsb = 0;

    result = bcm_field_direct_extraction_action_add(unit,action_entry,extract,1,&newAppType_action);
    if (BCM_E_NONE != result) {
        printf("Error in field_fwd_ipv6UC_uc_with_rpf_2pass calling bcm_field_direct_extraction_action_add Err %x\n",result);
        return result;
    }
    else {
        printf("Success in field_fwd_ipv6UC_uc_with_rpf_2pass calling bcm_field_direct_extraction_action_add action %x\n",action_entry);
    }

    result = bcm_field_group_install(unit, fwd_code_group.group);	
    if (BCM_E_NONE != result) {
        printf("Error in field_fwd_ipv6UC_uc_with_rpf_2pass calling bcm_field_group_install Err %x\n",result);
        return result;
    }
    else
    {
        printf("Success in field_fwd_ipv6UC_uc_with_rpf_2pass calling bcm_field_group_install group %x\n",fwd_code_group.group);
    }
 
    *field_presel_id = presel_id;

    printf("field_fwd_ipv6UC_uc_with_rpf_2pass: Field Group Setting action update FWD_CODE IPV6->IPV4 Done\n");
    bcm_field_group_dump(unit,fwd_code_group.group);

    return result;
}


