
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.3
*/

/*
 *  The requirement is to count all received and transmitted packets,
 *  per OAM MEP/session. Implementation should support all formats (Eth-OAM and BFD).
 * 
 *  The mapping of generated packets to a counter pointer is done by a LEM lookup.
 *  The PMF assign one of the counter pointers from this lookup result
 *
 *  This cint adds the PMF rules for the oam per mep counting
 *  
 * 
 *  To Activate Above Settings:
 *  1. set the soc property: oam_statistics_per_mep_enabled=1
 *  2. Configure the counter processor as described in user manual
 */

/* in case of up mep session the opaque is equal to 7*/
int oam_tx_up_stat(int unit) {
	bcm_field_group_config_t grp;
	bcm_field_presel_set_t presel_set;
	int presel_id = 0;
    int presel_flags = 0;

	bcm_field_aset_t aset;

	bcm_field_entry_t action_entry;

	bcm_field_stat_t stats = bcmFieldStatPackets;
	int statId;
	

	int sys_gport;
	bcm_field_stage_t stage;
	bcm_field_data_qualifier_t stat_id_qualifier;


	int result = 0;

	/*create preselector*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create_stage_id\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
          return result;
      } else {
		printf("bcm_field_presel_create_stage_id: %x\n", presel_id);
	  }
    } else {
      result = bcm_field_presel_create(unit, &presel_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id);
          return result;
      } else {
		printf("bcm_field_presel_create: %x\n", presel_id);
	  }
    }

	BCM_FIELD_PRESEL_INIT(presel_set);

	/*add match criteria to the preselectors*/
	/*supported only in ingress*/
	stage = bcmFieldStageIngress;
	result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_qualify_Stage Err %x\n", result);
		return result;
	}

    /* ARAD+:   preselector is OAMP UP MEP packets and hit in LEM
     * Jericho: preselector is OAMP UP MEP packets only*/
    if(!is_device_or_above(unit, JERICHO))
    {
       /* match criteria acording to the 2nd LEM lookup*/
       result = bcm_field_qualify_L2SrcHit(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 1, 1);
       if (BCM_E_NONE != result) {
           printf("Error in bcm_field_qualify_L2SrcHit Err %x\n", result);
           return result;
       }
    }

    /* match criteria Packet-Format-Qualifier[0] for oam up mep = 7*/
    result = bcm_field_qualify_Ptch(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x7, 0x7);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Ptch Err %x\n", result);
        return result;
    }

    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);

    /*create user define qualifier
     * ARAD+:   counter pointer is LEM result
     * Jericho: counter pointer is 2 bytes after PTCH header*/
    bcm_field_data_qualifier_t_init(&stat_id_qualifier);
    if(is_device_or_above(unit, JERICHO))
    {
        stat_id_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_WITH_ID;
        stat_id_qualifier.offset = 2;
        stat_id_qualifier.length =  16; /* bits */
        stat_id_qualifier.offset_base = bcmFieldDataOffsetBasePacketStart;
        stat_id_qualifier.qualifier = 32;
    } else {
        stat_id_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
        stat_id_qualifier.offset = 0;
        stat_id_qualifier.length = 16; /* bits */
        stat_id_qualifier.qualifier = bcmFieldQualifyL2SrcValue;
    }
	result = bcm_field_data_qualifier_create(unit, &stat_id_qualifier);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_data_qualifier_create - stat_id_qualifier Err %x\n", result);
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
	grp.priority = 8;


	/*add qualifiers*/
	BCM_FIELD_QSET_INIT(grp.qset);

	result = bcm_field_qset_data_qualifier_add(unit,
											   grp.qset,
											   stat_id_qualifier.qual_id);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
		return result;
	}

	BCM_FIELD_ASET_INIT(aset);
    if(soc_property_get(unit , "oam_statistics_per_mep_enabled",1) == 2) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat1);
    } else {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat0);
    }

	/* create field group */
	result = bcm_field_group_config_create(unit, &grp);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_group_config_create with Err %x\n", result);
		return result;
	}
	
	result = bcm_field_group_action_set(unit, grp.group, aset);
	if (BCM_E_NONE != result) {
		auxRes = bcm_field_group_destroy(unit, grp);
		printf("Set 1 exit \n");
		return result;
	}

	/*create an entry*/
	result = bcm_field_entry_create(unit, grp.group, &action_entry);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_entry_create Err %x\n", result);
		return result;
	}

	bcm_field_extraction_action_t extract;
	bcm_field_extraction_field_t ext_counter[2];

	bcm_field_extraction_action_t_init(&extract);
	bcm_field_extraction_field_t_init(&ext_counter);

    if(soc_property_get(unit , "oam_statistics_per_mep_enabled",1) == 2)
        extract.action = bcmFieldActionStat1;
    else
        extract.action = bcmFieldActionStat0;
	extract.bias = 0;
if(is_device_or_above(unit, JERICHO))
{
	/* First extraction structure indicates action is valid */
    ext_counter[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_counter[0].bits  = 2;
    ext_counter[0].value = 1;
} else {
    /* First extraction structure indicates action is valid */
      ext_counter[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
      ext_counter[0].bits  = 1;
      ext_counter[0].value = 1;
}

	  /* second extraction structure indicates to use counter ID */
	ext_counter[1].bits = 16;
	ext_counter[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
	ext_counter[1].lsb = 0;
	ext_counter[1].qualifier = stat_id_qualifier.qual_id;

	result = bcm_field_direct_extraction_action_add(unit, action_entry, extract, 2, &ext_counter);
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

/* rx down and bfd packets */
int oam_rx_down_stat(int unit) {

    bcm_field_group_config_t grp;
    bcm_field_presel_set_t presel_set;
    int presel_id = 0;
    int presel_flags = 0;
    bcm_field_aset_t aset;
    bcm_field_entry_t action_entry;
    bcm_field_stat_t stats = bcmFieldStatPackets;
    int statId;
    int sys_gport;
    bcm_field_stage_t stage;
    bcm_field_data_qualifier_t stat_id_qualifier;
    int trap_code;
    int result = 0;

    /*create preselector*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create_stage_id\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
          return result;
      }
    } else {
      result = bcm_field_presel_create(unit, &presel_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id);
          return result;
      }
    }

    BCM_FIELD_PRESEL_INIT(presel_set);

    /*add match criteria to the preselectors*/
    /*supported only in ingress*/
    stage = bcmFieldStageIngress;
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage Err %x\n", result);
        return result;
    }

    /* Get the trap code used for OAM traps*/
    result = bcm_rx_trap_type_get(unit,0/* flags */ ,bcmRxTrapOamBfdIpv4, &trap_code);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_trap_type_get Err %x\n",result);
        return result;
    }

    /* match criteria acording to the HW trap code - bcmRxTrapOamBfdIpv4*/
    result = bcm_field_qualify_RxTrapCode32(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, trap_code, 0xff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_RxTrapCode32 Err %x\n",result);
        return result;
    }

    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);

    bcm_field_data_qualifier_t_init(&stat_id_qualifier);
    stat_id_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    stat_id_qualifier.offset = 0;
    stat_id_qualifier.length = 16; /* bits */
    stat_id_qualifier.qualifier = bcmFieldQualifyOamMepId;
    result = bcm_field_data_qualifier_create(unit, &stat_id_qualifier);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create - stat_id_qualifier Err %x\n", result);
        return result;
    }

    result = bcm_field_data_qualifier_create(unit, &stat_id_qualifier);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create - stat_id_qualifier Err %x\n", result);
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
    grp.priority = 5;

    /*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               stat_id_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    BCM_FIELD_ASET_INIT(aset);
    if(soc_property_get(unit , "oam_statistics_per_mep_enabled",1) == 2) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat1);
    } else {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat0);
    }

    /* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n", result);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp);
        printf("Set 1 exit \n");
        return result;
    }

    /*create an entry*/
    result = bcm_field_entry_create(unit, grp.group, &action_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_counter[2];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_counter);

    if(soc_property_get(unit , "oam_statistics_per_mep_enabled",1) == 2)
        extract.action = bcmFieldActionStat1;
    else
        extract.action = bcmFieldActionStat0;
    extract.bias = 0;

    /* First extraction structure indicates action is valid */
      ext_counter[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
      ext_counter[0].bits  = 2;
      ext_counter[0].value = 3;

    /* second extraction structure indicates to use coumter ID */
    ext_counter[1].bits = 16;
    ext_counter[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_counter[1].lsb = 0;
    ext_counter[1].qualifier = stat_id_qualifier.qual_id;

    result = bcm_field_direct_extraction_action_add(unit, action_entry, extract, 2, &ext_counter);
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

