/* $ID: cint_field_key_a.c, v 1.1 2016/01/26 skoparan$
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_field_key_a.c
 * Purpose: Cint for testing the usage of key-A
 *
 * Main functions to be called from DVAPI:
 * ---------------------------------------
 *  run_fg_add(int unit, int is_single, int group_id, int group_priority)
 *      creates 1 field group with 1 entry.
 *      if is_single set, the group will use single TCAM bank (Key-A group)
 *
 *  run_entry_add(int unit, int group_id, int group_priority, int num)
 *      creates 1 field group with *num* entries
 *      group is for key-A
 *
 * Cleanup functions:
 * ------------------
 *  field_group_destroy(int unit, int group_id)
 *      destroys the group with the specified group_id
 *
 *  field_groups_destroy_all(int unit)
 *      destroys all entries and all field groups, using traverse callback
 */

/*destroy selected field group with its entries*/
bcm_error_t field_group_destroy(int unit, int group_id) {
    bcm_error_t rv = BCM_E_NONE;
    rv = bcm_field_group_flush(unit, group_id);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_flush failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Traverse callback to destroy all groups created*/
bcm_error_t field_group_destroy_callback(int unit, 
    bcm_field_group_t group, 
    void *user_data) {
    bcm_error_t rv = BCM_E_NONE;

    printf("Flushing...\n");
    rv = bcm_field_group_flush(unit, group);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_flush failed (%s)\n", bcm_errmsg(rv));
    }

    return rv;
}

/* Function to cleanup existing groups, using traverse */
bcm_error_t field_groups_destroy_all(int unit) {
    bcm_error_t rv = BCM_E_NONE;
    int ud = 0;

    rv = bcm_field_group_traverse(unit, field_group_destroy_callback, &ud);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_traverse failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Key-A usage Cint */
bcm_error_t run_fg_add(int unit, int is_single, int group_id, int group_priority) {
    bcm_error_t rv = BCM_E_NONE;
    int group_num = 0;
    bcm_field_group_config_t group;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_mac_t macData = {0x00,0x01,0x02,0x03,0x04,0x05};
    bcm_mac_t macMask = {0xff,0xff,0xff,0xff,0xff,0xff};
    int flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    flags = flags | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    flags = flags | BCM_FIELD_GROUP_CREATE_WITH_ID;

    printf("flags = %d\n", flags);
    if(is_single == 1) {
        flags = flags | BCM_FIELD_GROUP_CREATE_SINGLE;
    }
    printf("flags = %d\n", flags);

    /* Create field group with QSET and ASET */
    bcm_field_group_config_t_init(&group);

    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageIngress);

    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionDoNotLearn);

    group.group = group_id;
    group.priority = group_priority;
    group.flags = flags;
    group.mode = bcmFieldGroupModeAuto;

    rv = bcm_field_group_config_create(unit, &group);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_config_create failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    printf("FIELD GROUP CREATED:\n-~-~-~-~-~-~-~-~-~-\n");
    rv = bcm_field_group_dump(unit, group_id);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_dump failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_create(unit, group.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_entry_create failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    rv =  bcm_field_qualify_DstMac(unit, ent, macData, macMask);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_qualify_DstMac failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    if(is_single == 1) {
        rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    } else {
        rv = bcm_field_action_add(unit, ent, bcmFieldActionDoNotLearn, 0, 0);
    }
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_action_add failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_entry_install failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    printf("FIELD ENTRY CREATED:\n-~-~-~-~-~-~-~-~-~-\n");
    rv = bcm_field_entry_dump(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_entry_dump failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Main function to add key-A group with multiple entries */
bcm_error_t run_entry_add(int unit, int group_id, int group_priority, int num) {
    bcm_error_t rv = BCM_E_NONE;
    int fg_flags = BCM_FIELD_GROUP_CREATE_WITH_MODE |
                   BCM_FIELD_GROUP_CREATE_WITH_ID |
                   BCM_FIELD_GROUP_CREATE_WITH_ASET |
                   BCM_FIELD_GROUP_CREATE_SINGLE;
    bcm_field_entry_t ent;
    int n;

    bcm_mac_t macData = {0x00,0x01,0x02,0x03,0x04,0x05};
    bcm_mac_t macMask = {0xff,0xff,0xff,0xff,0xff,0xff};
    bcm_mac_t macNew = {0x99,0x01,0x02,0x03,0x04,0x05};

    bcm_field_group_config_t group;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;

    BCM_FIELD_QSET_INIT(qset);
 
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassSourceSet);

    bcm_field_group_config_t_init(&group);
    group.group = group_id;
    group.priority = group_priority;
    group.mode = bcmFieldGroupModeAuto;

    group.flags = fg_flags;
    group.aset = aset;
    group.qset = qset;
    rv = bcm_field_group_config_create(unit,&group);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_config_create failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Adding %d entries\n", num);
    for(n = 0; n < num; n++) {
        rv = bcm_field_entry_create(unit, group.group, &ent);
        if (rv != BCM_E_NONE) {
            printf("Error: bcm_field_entry_create failed (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    rv = bcm_field_group_install(unit, group.group);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_install failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Adding one more entry\n");
    rv = bcm_field_entry_create(unit, group.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_entry_create failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_group_install(unit, group.group);
    if (rv != BCM_E_FULL) {
        printf("Error: bcm_field_group_install exit code not as expected (Table full), but (%s)\n", bcm_errmsg(rv));
        rv = BCM_E_FAIL;
        return rv;
    }
    rv = BCM_E_NONE;
    return rv;
}
