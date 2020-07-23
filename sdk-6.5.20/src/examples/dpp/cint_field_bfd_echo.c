/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_field_bfd_echo.c
 * Purpose: pmf stage in bfd echo solution.
 *  	stamp the trap qualifier to be the oam_id
 *  	stamp the oam offset to be equal to the start of BFD header
 *
 * Usage:
 * Enable soc property bfd_echo_enabled=1 custom_feature_bfd_echo_with_lem=1 (together with all other oam soc properties)
 * 
 * Add PMF rule using cint: cint_field_bfd_echo.c function:bfd_echo_with_lem_preselector
 * 
 * Add bfd endpoint by calling bcm_bfd_endpoint_create with: 
 * flags |= BCM_BFD_ECHO & BCM_BFD_ENDPOINT_MULTIHOP
 */
/********** 
  functions
 */
int bfd_ipv4_grp_pri_1 = 0;
int bfd_ipv4_grp_pri_2 = 1;
int bfd_ipv4_grp_pri_3 = 2;

int bfd_echo_with_lem_preselector(int unit)
{
    bcm_field_presel_set_t presel_set;  
    int presel_id = 0;
    int presel_id2 = 1;
    int presel_flags = 0;

    bcm_field_stage_t stage;

    int sys_gport;

    int result=0;

    /*create preselector*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
        else {
            printf("bcm_field_presel_create_stage_id: %x\n", presel_id);
        }
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id2);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id2);
            return result;
        }
        else {
            printf("bcm_field_presel_create_stage_id: %x\n", presel_id2);
        }
    } else {
        result = bcm_field_presel_create(unit, &(presel_id));
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
        else {
            printf("bcm_field_presel_create: %x\n", presel_id);
        }
        result = bcm_field_presel_create(unit, &(presel_id2));
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id2);
            return result;
        }
        else {
            printf("bcm_field_presel_create: %x\n", presel_id2);
        }
    }
	
    BCM_FIELD_PRESEL_INIT(presel_set);

    /*add match criteria to the preselectors*/
    /*supported only in ingress*/
    stage = bcmFieldStageIngress;
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage Err %x\n",result);
        return result;
    } 
   
    /* match criteria acording to the HW trap code - bcmRxTrapBfdEchoOverIpv4 */
    result = bcm_field_qualify_RxTrapCode32(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 227, 0xff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_RxTrapCode32 Err %x\n",result);
        return result;
    }  

    /* match criteria acording to the 2nd LEM lookup*/
    result = bcm_field_qualify_L2DestHit(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 1,1); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_L2DestHit Err %x\n",result);
        return result;
    }

    /* match criteria Packet-Format-Qualifier[0] for not injected packet*/
    result = bcm_field_qualify_Ptch(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0,0x7); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Ptch Err %x\n",result);
        return result;
    } 

    result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeIp4Ucast);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_AppType\n");
        return result;
    }

    bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldAppTypeIp4Ucast);
    if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_ForwardingType\n");
          return result;
    }

 
    BCM_FIELD_PRESEL_ADD(presel_set, presel_id); 

    
    result= bfd_echo_field_trap_qualifier(unit,presel_set);
    if (BCM_E_NONE != result) {
            printf("Error in bfd_ipv4_single_hop_field_trap_qualifier Err %x\n",result);
            return result;
    } 

    result= bfd_echo_field_oam_action(unit,presel_set);
    if (BCM_E_NONE != result) {
            printf("Error in bfd_ipv4_single_hop_field_oam_action Err %x\n",result);
        return result;
    } 



    return result;

}






/* this field group set the OAM offset to be equal to the start of the BFD header*/

int bfd_echo_field_oam_action(int unit, bcm_field_presel_set_t presel_set)
{

	bcm_field_group_config_t grp_oam_always;

	bcm_field_aset_t aset_oam_always;

	bcm_field_entry_t oam_entry;

	int sys_gport;
	bcm_field_stage_t stage;
	bcm_field_data_qualifier_t offset_qualifier;
	int result=0;


	/*create user define quelifier*/

	
	/* offset header 4 */
	bcm_field_data_qualifier_t_init(&offset_qualifier); 
	offset_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
	offset_qualifier.offset = 32; 
	offset_qualifier.length = 7; 
	offset_qualifier.qualifier = bcmFieldQualifyForwardingHeaderOffset;
	result = bcm_field_data_qualifier_create(unit, &offset_qualifier);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_data_qualifier_create - offset_qualifier Err %x\n",result);
		return result;
	} 



    /* Initialized Field Group */
	bcm_field_group_config_t_init(&grp_oam_always);
	
    /* 
     * Define Programabble Field Group
     */ 
	grp_oam_always.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET; 
	grp_oam_always.mode = bcmFieldGroupModeDirectExtraction;
    grp_oam_always.preselset = presel_set;
	grp_oam_always.priority = bfd_ipv4_grp_pri_2; 

	/*add qualifiers*/
	BCM_FIELD_QSET_INIT(grp_oam_always.qset);

	result = bcm_field_qset_data_qualifier_add(unit,
                                             grp_oam_always.qset,
                                             offset_qualifier.qual_id);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_qset_data_qualifier_add Err %x\n",result);
        return result;
	} 

	/* create field group */
	result = bcm_field_group_config_create(unit, &grp_oam_always);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_group_config_create with Err %x\n",result);
		return result;
	}


	BCM_FIELD_ASET_INIT(aset_oam_always);
	BCM_FIELD_ASET_ADD(aset_oam_always, bcmFieldActionOam);

	/* Attached the action to the field group */
	result = bcm_field_group_action_set(unit, grp_oam_always.group, aset_oam_always);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n",result);
        return result;
    }

	
 
     
	/*create an entry*/
	result = bcm_field_entry_create(unit, grp_oam_always.group, &oam_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n",result);
        return result;
    }



	/* set oam offset*/
	bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t oam_offset_action[3];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&oam_offset_action);

    extract.action = bcmFieldActionOam;
    extract.bias = 0;

	oam_offset_action[0].bits = 7;  
    oam_offset_action[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    oam_offset_action[0].value = 0; 

	oam_offset_action[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
	oam_offset_action[1].bits  = 7;
	oam_offset_action[1].lsb = 0;
    oam_offset_action[1].qualifier = offset_qualifier.qual_id; 

	oam_offset_action[2].bits = 4;  
    oam_offset_action[2].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    oam_offset_action[2].value = 0; 

	/*new entry*/
	result = bcm_field_direct_extraction_action_add(unit,oam_entry,extract,3,&oam_offset_action);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_direct_extraction_action_add\n");
		return result;
	}
	

	result = bcm_field_group_install(unit, grp_oam_always.group);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_group_install\n");
		return result;
	}

    return result;

}






/* this field group set the trap qualifier to be the oam_id*/

int bfd_echo_field_trap_qualifier(int unit, bcm_field_presel_set_t presel_set)
{
   
    bcm_field_group_config_t grp;

    bcm_field_aset_t aset;
	
    bcm_field_entry_t action_entry;

	int sys_gport;
	bcm_field_stage_t stage;
	bcm_field_data_qualifier_t outlif_qualifier;
	bcm_field_data_qualifier_t trap_code;
	bcm_field_data_qualifier_t strength;
	bcm_field_data_qualifier_t udp_qualifier;
	
	
	int result=0;

	bcm_field_range_t range;
	
	uint32 flags;
	uint32 minLen;
	uint32 maxLen;



	

	/*create user define quelifier*/

	bcm_field_data_qualifier_t_init(&trap_code);
	trap_code.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
	trap_code.offset = 0; /* points to the lsb of you.descriminator */
	trap_code.length = 8; /* bits */
	trap_code.qualifier = bcmFieldQualifyRxTrapCode;
	result = bcm_field_data_qualifier_create(unit, &trap_code);
	if (BCM_E_NONE != result) {
	printf("Error in bcm_field_data_qualifier_create - trap_code Err %x\n",result);
	return result;
	}


	bcm_field_data_qualifier_t_init(&strength); 
	strength.flags =  BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
	strength.offset = 0; /* points to the lsb of you.descriminator */
	strength.length = 3; /* bits */
	strength.qualifier = bcmFieldQualifyConstantOne; 
	result = bcm_field_data_qualifier_create(unit, &strength);
	if (BCM_E_NONE != result) {
	printf("Error in bcm_field_data_qualifier_create - strength Err %x\n",result);
	return result;
	}

	bcm_field_data_qualifier_t_init(&outlif_qualifier); 
	outlif_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
	outlif_qualifier.offset = 24;
	outlif_qualifier.length = 16; /* bits */
	outlif_qualifier.qualifier = bcmFieldQualifyL2DestValue;
	result = bcm_field_data_qualifier_create(unit, &outlif_qualifier);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_data_qualifier_create - outlif_qualifier Err %x\n",result);
		return result;
	}

	/* UDP.Src-Port[15:14] = 2b11 */
	bcm_field_data_qualifier_t_init(&udp_qualifier); 
	udp_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
	udp_qualifier.offset = 0; 
	udp_qualifier.length = 1; 
	udp_qualifier.qualifier = bcmFieldQualifyL4PortRangeCheck;
	result = bcm_field_data_qualifier_create(unit, &udp_qualifier);
	if (BCM_E_NONE != result) {
		print bcm_field_show(unit,"");
		return result;
	}

	


    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);
	
    /* 
     * Define Programabble Field Group
     */ 
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET; 
	grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = presel_set;
	grp.priority = bfd_ipv4_grp_pri_1;
	
	
	/*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);
	

	
	
	result = bcm_field_qset_data_qualifier_add(unit,
                                             grp.qset,
                                             trap_code.qual_id);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_qset_data_qualifier_add Err %x\n",result);
        return result;
	}
	
	
	
	result = bcm_field_qset_data_qualifier_add(unit,
                                             grp.qset,
                                             strength.qual_id);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_qset_data_qualifier_add Err %x\n",result);
        return result;
	}


	result = bcm_field_qset_data_qualifier_add(unit,
											 grp.qset,
											 outlif_qualifier.qual_id);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_qset_data_qualifier_add Err %x\n",result);
		return result;
	}

	result = bcm_field_qset_data_qualifier_add(unit,
                                             grp.qset,
                                             udp_qualifier.qual_id);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_qset_data_qualifier_add Err %x\n",result);
        return result;
	} 



	/* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n",result);
        return result;
    }

	


	BCM_FIELD_ASET_INIT(aset);
	
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);
	

	/* Attached the action to the field group */
	

    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n",result);
        return result;
    }

	
	
 
     
	/*create an entry*/
    result = bcm_field_entry_create(unit, grp.group, &action_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n",result);
        return result;
    }



	int i=0;
	flags = BCM_FIELD_RANGE_UDP  | BCM_FIELD_RANGE_SRCPORT; 
	minLen = 49152;
	maxLen = 65535;

	result = bcm_field_range_multi_create(unit,
									  &(range),
									  0,
									  1,
									  &(flags),
									  &(minLen),
									  &(maxLen));
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_range_multi_create Err %x\n",result);
		return result;
	}

    uint8 mask=0x1;
	uint8 data=0x1;

	result = bcm_field_qualify_data(unit, action_entry, udp_qualifier.qual_id, &data, &mask, 1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    } 




	
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t new_trap_action[4];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&new_trap_action);

    extract.action = bcmFieldActionTrap;
    extract.bias = 0;

	new_trap_action[0].bits = 1;  
    new_trap_action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    new_trap_action[0].lsb = 0;
    new_trap_action[0].qualifier = udp_qualifier.qual_id;

	new_trap_action[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
	new_trap_action[1].bits  = 8;
	new_trap_action[1].lsb = 0;
    new_trap_action[1].qualifier = trap_code.qual_id;

	new_trap_action[2].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
	new_trap_action[2].bits  = 3;
	new_trap_action[2].lsb = 0;
    new_trap_action[2].qualifier = strength.qual_id;

    new_trap_action[3].bits = 16;  
    new_trap_action[3].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    new_trap_action[3].lsb = 0;
    new_trap_action[3].qualifier = outlif_qualifier.qual_id;



    result = bcm_field_direct_extraction_action_add(unit,action_entry,extract,4,&new_trap_action);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_direct_extraction_action_add\n");
		return result;
	}

	
	result = bcm_field_group_install(unit, grp.group);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_group_install\n");
		return result;
	}

    return result;

}




