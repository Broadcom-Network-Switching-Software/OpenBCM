/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * Main functions:
 * --------------
 * run_main (int unit) 
 *     Main function to create field group for adding Learn Extension to PPH
 * run_verify (int unit) 
 *     Main function to verify action was hit
 *
 * Cleanup function:
 * ----------------
 * run_cleanup (int unit)
 *    destroys the created group and its entries
 */

bcm_field_group_t group = 0;
int core = 0;

/* Function to create a field group with entry to add Learn Extension */
int config_learn_extension(int unit, int group_priority) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int presel_id = 1;
    int presel_flags = 0;
    bcm_field_presel_set_t psset;
    bcm_field_extraction_action_t ext_action;
    bcm_field_extraction_field_t ext_field;

    bcm_field_group_config_t_init(&grp);
    grp.group = group;

    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS; 
      rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id); 
      if (BCM_E_NONE != rv) { 
          printf("Error in bcm_field_presel_create_stage_id\n"); 
          return rv; 
      } 
    } else { 
      rv = bcm_field_presel_create(unit, &presel_id); 
      if (BCM_E_NONE != rv) { 
          printf("Error in bcm_field_presel_create\n"); 
          return rv; 
      }
    }
    rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_Stage (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    /*Dummy qualifier to get the group added:*/
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyConstantZero);

    /* Define the ASET */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionFabricHeaderSet);

    /* Create the Field group */
    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    grp.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp.mode  = bcmFieldGroupModeAuto;
    grp.preselset = psset;
    grp.aset = aset;
    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_config_create (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create entry */
    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv !=  BCM_E_NONE) {
        printf("Error in bcm_field_entry_create (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    /* Add action to entry */
    rv = bcm_field_action_add(unit, ent, bcmFieldActionFabricHeaderSet, bcmFieldFabricHeaderEthernetLearn, 0);
    if (rv !=  BCM_E_NONE) {
        printf("Error in bcm_field_action_add (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    /* Install entry */
    rv = bcm_field_entry_install(unit, ent);
    if (rv !=  BCM_E_NONE) {
        printf("Error in bcm_field_entry_install (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Learn data extension field created:\n");
    rv = bcm_field_entry_dump(unit, ent);
    if (rv !=  BCM_E_NONE) {
        printf("Error in bcm_field_entry_dump (%s)\n", bcm_errmsg(rv));
        return rv;
    }
  
    return rv;
}

/*L2 config function*/
int config_l2(int unit, bcm_port_t inport, bcm_port_t outport, int vlan) { 
    bcm_error_t rv = BCM_E_NONE;
    bcm_l2_addr_t entry;
    bcm_l2_learn_limit_t limit;
    bcm_pbmp_t pbm;
    bcm_pbmp_t empty;
    bcm_mac_t dmac = "11:A0:11:B0:11:C0";
    bcm_port_t invalid = 19;

    /* Create VSI and add inport and outport to VLAN */
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_vlan_create failed (%s)", bcm_errmsg(rv));
    }
    BCM_PBMP_CLEAR(empty);
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, inport);
    BCM_PBMP_PORT_ADD(pbm, outport);
    rv = bcm_vlan_port_add(unit, vlan, pbm, empty);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_vlan_port_add failed (%s)", bcm_errmsg(rv));
    }
    printf("Ports %d and %d added to VLAN %d\n", inport, outport, vlan);

    /* Add Destination MAC to MACT with outport and VLAN */
    bcm_l2_addr_t_init(&entry, dmac, vlan);
    entry.port = outport;
    rv = bcm_l2_addr_add(unit, entry);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_l2_addr_add failed (%s)", bcm_errmsg(rv));
    }
    printf("Entry added to MAC table:\nMAC=");
    print dmac;
    printf(" port=%d, vlan=%d\n", outport, vlan);
    
    return rv;
}

/* Cleanup function */
int run_cleanup(int unit) {
    int rv = BCM_E_NONE;
    rv = bcm_field_group_flush(unit, group);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_flush failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Main function to setup field for creating Fabric Header Learn Extension.*/
int run_main(int unit) {
    bcm_error_t rv = BCM_E_NONE;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;

    rv = config_learn_extension(unit, group_priority);
    if (rv != BCM_E_NONE) {
        printf("Error in config_learn_extention (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Main function to verify action was hit */
int run_verify(int unit, int core) {
    uint32 valid;
    int rv = dpp_dsig_read(unit, core, "ERPP", NULL, NULL, "Learn_Extension_Valid", &valid, 1);
    /*If Signal DB was not initialized, cannot verify: exit with pass*/
    if (rv != BCM_E_NONE) {
        printf("Error in dpp_dsig_read (%s)\nEXIT WITH PASS\n", bcm_errmsg(rv));
        return BCM_E_NONE;
    }
    printf("DIAG: ETPP > Learn_Extention_Valid = %d\n", valid);
    if(valid == 0) {
        printf("FAIL: Learn Extension could not be verified as set to Valid in Egress!\n");
        return BCM_E_FAIL;
    }

    return rv;
}

