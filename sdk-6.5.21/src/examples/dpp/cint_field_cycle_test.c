/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_cycle_test.c
 * Purpose: Example test cycle.
 *
 * Fuctions:
 * Main function:
 *      field_group_cycle_1_test()
 *
 * Cleanup function:
 *      field_group_cycle_1_destroy()
 */

int field_group_cycle_1_test(int unit, bcm_field_group_t group, int group_priority, bcm_field_group_cycle_t cycle, bcm_gport_t in_port) {
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    int rv;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp_conf;


    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    bcm_field_group_config_t_init(&grp_conf);
    grp_conf.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_CYCLE;
    grp_conf.mode = bcmFieldGroupModeAuto;
    grp_conf.qset = qset;
    grp_conf.priority = group_priority;
    grp_conf.group = group;
    grp_conf.cycle=1;

    rv = bcm_field_group_config_create(unit, &grp_conf);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    rv = bcm_field_group_action_set(unit, group, aset);
    if (BCM_E_NONE != rv) {
        bcm_field_group_destroy(unit, group);
        printf("Error in bcm_field_group_action_set\n");
        return rv;
    }

    rv = bcm_field_entry_create(unit, group, &ent);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_create\n");
        bcm_field_group_destroy(unit, group);
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit,ent,in_port,0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_InPort\n");
        bcm_field_entry_destroy(unit, ent);
        bcm_field_group_destroy(unit, group);
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_action_add\n");
        bcm_field_entry_destroy(unit, ent);
        bcm_field_group_destroy(unit, group);
        return rv;
    }

    rv = bcm_field_group_install(unit, group);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_install\n");
        bcm_field_entry_destroy(unit, ent);
        bcm_field_group_destroy(unit, group);
        return rv;
    }

    return rv;
}

/*
Function to destroy the configured field group and entry
*/
int field_group_cycle_1_destroy(int unit, bcm_field_group_t group) {
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
