/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/**
 * This CINT script is an PMF example to extract ARP(14) in LEM payload to OutLIF for router over AC functions. 
 *  1. Setup field group preslector for IP packets in downsteam.
 *      print field_group_router_over_ac_pselect_setup(unit, nni_port);
 *  2. Setup field group to extract ARP(14) in LEM payload to OutLIF
 *     print field_group_router_over_ac_setup(unit, pre_selector, group_priority, &group)
 */


bcm_field_presel_set_t psset_router_over_ac;
int presel_id_router_over_ac = 0;
/* Create a pselect for router over ac in downstream.
 * params:
 *    unit: device number
 *    in_pbmp: bitmap for NNI port.
 */
int field_group_router_over_ac_pselect_setup(/* in */int unit, 
                                             /* in */int nni_port)
{
    int result;
    int auxRes;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int presel_flags = 0;

    /* 
     * Set the preselector to be on the ports
     */
    
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, nni_port);

    for(i = 0; i < 256; i++)
    { 
        BCM_PBMP_PORT_ADD(pbm_mask, i);
    }

    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id_router_over_ac);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_router_over_ac | presel_flags);
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id_router_over_ac);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_router_over_ac | presel_flags);
            return result;
        }
    }
  
    result = bcm_field_qualify_Stage(unit, presel_id_router_over_ac | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_Stage\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_router_over_ac | presel_flags);
        return result;
    }
    
    result = bcm_field_qualify_InPorts(unit, presel_id_router_over_ac | BCM_FIELD_QUALIFY_PRESEL | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_router_over_ac | presel_flags);
        return result;
    }
  
    /* IPv4 packet  
     * bcmFieldIpTypeIpv4Any is working in current master codes, comment it temporary.
     */
    /*result = bcm_field_qualify_IpType(unit, presel_id_router_over_ac | BCM_FIELD_QUALIFY_PRESEL,  bcmFieldIpTypeIpv4Any);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_EtherType\n");
        auxRes = bcm_field_qualify_IpType(unit, presel_id_router_over_ac);
        return result;
    }*/

    /* IP host hit */
    result = bcm_field_qualify_L2DestHit(unit, presel_id_router_over_ac | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_L2DestHit\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_router_over_ac | presel_flags);
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset_router_over_ac);
    BCM_FIELD_PRESEL_ADD(psset_router_over_ac, presel_id_router_over_ac);

    return result;
}


/*
 *  Set the Field group that:
 *  -Extract ARP(14) in LEM payload to OutLIF using PMF.
 */
bcm_field_group_t group = 0;
int field_group_router_over_ac_setup(/* in */int unit,
                                              /* in */  int group_priority)
{
    bcm_field_aset_t aset;
	bcm_field_data_qualifier_t outlif_qualifier;
	int result = 0;
    bcm_field_group_config_t grp;
    bcm_field_entry_t action_entry;


    /* LEM payload - RIF(12),HI(4),ARP(14),FEC(12) */
	bcm_field_data_qualifier_t_init(&outlif_qualifier); 
	outlif_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
	outlif_qualifier.offset = 12;
	outlif_qualifier.length = 14; /* bits */
	outlif_qualifier.qualifier = bcmFieldQualifyL2DestValue;
	result = bcm_field_data_qualifier_create(unit, &outlif_qualifier);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_data_qualifier_create - outlif_qualifier Err %x\n",result);
		return result;
	}


    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);
	
    /* 
     * Define Programabble Field Group
     */ 
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET; 
	grp.mode = bcmFieldGroupModeDirectExtraction;
	grp.priority = 2;
    grp.priority = group_priority;
    grp.preselset = psset_router_over_ac;

	/*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

	result = bcm_field_qset_data_qualifier_add(unit,
											 grp.qset,
											 outlif_qualifier.qual_id);
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
    group = grp.group;


	BCM_FIELD_ASET_INIT(aset);
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionVportNew);
	

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

    int nof_ext_actions = 2;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_action[nof_ext_actions];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_action);

    extract.action = bcmFieldActionVportNew;
    extract.bias = 0;

    ext_action[0].bits = 1;  
    ext_action[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_action[0].value = 0;

    ext_action[1].bits = 14;  
    ext_action[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_action[1].lsb = 0;
    ext_action[1].qualifier = outlif_qualifier.qual_id;
    

    result = bcm_field_direct_extraction_action_add(unit, action_entry, extract, nof_ext_actions, &ext_action);
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

/* Destory router over ac  Field group preslector resources */
int field_group_router_over_ac_pselect_teardown(/* in */int unit)
{
    int result = 0;

    result = bcm_field_presel_destroy(unit, presel_id_router_over_ac);
    if (BCM_E_NONE != result) {
    	printf("bcm_field_presel_destroy failed");
        return result;
    }

    return result;
}

/* Destory router over ac  Field group resources */
int field_group_router_over_ac_teardown(/* in */ int unit)
{
    int result;
    bcm_field_entry_t ent[128];
    int entCount;
    int entIndex;

    do {
        /* get a bunch of entries in this group */
        result = bcm_field_entry_multi_get(unit, group, 128, &(ent[0]), entCount);
        if (BCM_E_NONE != result) {
            return result;
        }
        for (entIndex = 0; entIndex < entCount; entIndex++) {
            /* remove each entry from hardware and destroy it */
            result = bcm_field_entry_remove(unit, ent[entIndex]);
            if (BCM_E_NONE != result) {
                return result;
            }
            result = bcm_field_entry_destroy(unit, ent[entIndex]);
            if (BCM_E_NONE != result) {
                return result;
            }
        }
    /* all as long as there were entries to remove & destroy */
    } while (entCount > 0);


    /* destroy the group */  
    result = bcm_field_group_destroy(unit, group);
    if (BCM_E_NONE != result) {
    	printf("bcm_field_group_destroy failed");
        return result;
    }

    return result;
}

