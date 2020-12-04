/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_small_banks.c
 * Purpose: Example creation of DBs in the TCAM small banks.
 *          The entry qualifies on SrcPort and applies action Drop.
 *          Can be applied with different group ID and priority
 *          to create several groups, up to the small banks limit.
 *
 * Fuctions:
 * Main function:
 *      field_config()
 *
 * Cleanup function:
 *      field_destroy()
 */

int field_config(int unit,
                 bcm_field_group_t group,
                 int group_priority,
                 bcm_port_t in_port) {
    int rv = BCM_E_NONE;
    bcm_field_group_config_t group_config;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_gport_t in_gport;
    bcm_field_action_t action = bcmFieldActionDrop;
    char *proc_name;
    
    proc_name = "field_config";
    printf("%s(): Configure field processor.\n", proc_name);

    /*Init group config structure*/
    bcm_field_group_config_t_init(&group_config);
    group_config.group = group;
    group_config.priority = group_priority;
    group_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_SMALL;
    group_config.mode = bcmFieldGroupModeQuad;
    /*Build the qset*/
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, action);

    /*Create group*/
    rv = bcm_field_group_config_create(unit, &group_config);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group_config.group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group_config.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    BCM_GPORT_LOCAL_SET(in_gport, in_port);
    rv = bcm_field_qualify_SrcPort(unit, ent, 0, 0, in_gport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_SrcPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_SrcPort.\n", proc_name);

    /*Set the action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    rv = bcm_field_action_add(unit, ent, action, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_action_add() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv; 
    }
    printf("%s(): bcm_field_action_add() done.\n", proc_name);

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group_config.group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);
    return rv;
}

/*
Function to destroy the configured fieldgroup and entry
*/
int field_destroy(int unit, bcm_field_group_t group) {
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "field_destroy";
    /*destroy group and entry*/
    rv = bcm_field_group_flush(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_flush(), returned %d\n", proc_name, rv);
        return rv;
    }

    printf("%s(): Done. Destroyed field group %d with its entries.\n", proc_name, group);
    return rv;
}

