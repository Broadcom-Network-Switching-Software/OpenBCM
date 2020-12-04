/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Purpose: Example of creating a data qualifier
 * and using it in a field group and entry
 *
 * Base qualifier used for the example: bcmFieldQualifyEthernetData
 *
 * This example creates 3 data qualifiers to match the Ethertype in Egress
 * for the 3 cases of an Ethernet header supported by Egress:
 *   1) Eth0 (no VLAN tags): Ethertype is at offset=0 in bcmFieldQualifyEthernetData
 *   2) Eth1 (one VLAN tag): Ethertype is at offset=32 in bcmFieldQualifyEthernetData
 *   3) Eth2 (two VLAN tags): Ethertype is at offset=64 in bcmFieldQualifyEthernetData
 *
 * A single field group is created, containing all 3 data qualifiers in its QSET.
 * 3 different entries are created to this group, each matching to one of the data qualifiers with the provided etype.
 *
 * Main function: config_field_egress_etype
 *  parameters:
 *      int unit (IN);
 *      bcm_field_group_t group (IN) : ID of the group to be created;
 *      int group_priority (IN) : Priority of the group to be created;
 *      int etype(IN) : The Ethernet type value to match the qualifier, up to 16 bits
 *
 * Destroy function: field_destroy
 *  parameters:
 *      int unit (IN);
 *      bcm_field_group_t group (IN) : ID of the group to be destroyed
 */

bcm_field_data_qualifier_t data_qualifier[3];

int config_field_egress_etype(int unit,
                        bcm_field_group_t group,
                        int group_priority,
                        int etype,
                        int counter_id,
                        int outP) {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    uint8 qual_data[2], qual_mask[2];
    int tag;
    bcm_gport_t local_gport;
    int32_t bcm_counter_id = counter_id;
    bcm_field_action_core_config_t core_config_arr[SOC_DPP_DEFS_GET_NOF_CORES(unit)] ;

    /* Fill the expected Ethertype data for the data qualifier*/
    qual_data[0] = etype >> 8; /* EType MSBs*/
    qual_data[1] = etype % 256; /* EType LSBs*/
    qual_mask[0] = 0xff;
    qual_mask[1] = 0xff;
    /*Create a data qualifier for each Ethernet tag format: 0, 1 or 2 tags*/
    for (tag = 0; tag <= 2; tag++) {
        bcm_field_data_qualifier_t_init(data_qualifier[tag]);
       
        data_qualifier[tag].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        data_qualifier[tag].qualifier = bcmFieldQualifyEthernetData;                                                           
        data_qualifier[tag].stage = bcmFieldStageEgress;
        data_qualifier[tag].offset = 32 * tag; /*For each tag there is 32 bits offset*/
        data_qualifier[tag].length = 16;
        rv = bcm_field_data_qualifier_create(0, &data_qualifier[tag]);
        if (rv != 0) {
             printf("1. bcm_field_data_qualifier_create failed. retval = %d\n", rv);
        }
        printf("Created data qualifier %d with ID %d\n", tag, data_qualifier[tag].qual_id);
    }
   
    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    /* Add the Data qualifier to the QSET */
    for(tag = 0; tag <= 2; tag++) {
        rv = bcm_field_qset_data_qualifier_add(unit, qset, data_qualifier[tag].qual_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_qset_data_qualifier_add \n");
            return rv;
        }
    }

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    /*
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    */
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_create_id returned %d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_action_set returned %d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    /*Create entry for each tag format, taking the corresponding data qualifier */
    for(tag = 0; tag <=2; tag++) {
        rv = bcm_field_entry_create(unit, group, &ent);
        if (rv != BCM_E_NONE) {
            printf("Error in : bcm_field_entry_create returned %d (%s)\n", rv, bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_field_qualify_data(unit, ent, data_qualifier[tag].qual_id, &qual_data, &qual_mask, 2);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        BCM_GPORT_LOCAL_SET(local_gport, outP);
        
        core_config_arr[0].param0 = bcm_counter_id;
        core_config_arr[0].param1 = local_gport;
		
		if( SOC_DPP_DEFS_GET_NOF_CORES(unit) == 2)
		{ 
			core_config_arr[1].param0 = bcm_counter_id;
            core_config_arr[1].param1 = local_gport;
		}
        
        
        bcm_field_action_config_add(unit,ent,bcmFieldActionStat0,2,&core_config_arr[0]);
        
        rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in : bcm_field_action_add() returned %d (%s)\n", rv, bcm_errmsg(rv));
            return rv; 
        } 
        
    }

    /*Commit the entire group to hardware.*/
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_install() returned %d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    return rv;
}

int field_destroy(int unit, int group) {
    int rv, i;
    rv = bcm_field_group_flush(unit,group);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_flush() returned %d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    for (i = 0; i < 3; i++) {
        rv = bcm_field_data_qualifier_destroy(unit, data_qualifier[i].qual_id);
        if (rv != BCM_E_NONE) {
            printf("Error in : bcm_field_data_qualifier_destroy() returned %d (%s)\n", rv, bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}
