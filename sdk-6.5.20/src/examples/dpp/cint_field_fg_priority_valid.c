/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * This cint verifies Field Group priority between actions Redirect and Drop on QAX
 */


bcm_port_t in_port=200;
bcm_port_t out_port=15;
bcm_field_group_t group1=5;
bcm_field_group_t group2=6;

bcm_field_entry_t entry1, entry2, entry3;


/*
 * FES(Redirect) vs FES(drop)
 * FG1: Drop with bcmFieldGroupModeAuto
 * FG2: Redirect with bcmFieldGroupModeAuto
 */
int field_group_priority_1(int unit, int priority1, int priority2)
{
    int result;
    group1 = 5;
    group2 = 6;
    int sys_gport;

    /*FG1*/
    bcm_field_group_config_t group;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    BCM_FIELD_QSET_INIT(&qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_INIT(&aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    group.group = group1;
    group.priority = priority1;
    group.qset = qset;
    group.aset = aset;
    group.mode = bcmFieldGroupModeAuto;
    group.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
    result = bcm_field_group_config_create(unit, &group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create for group %d\n", group.group);
        return result;
    }

    bcm_field_entry_t gr_entry;
    result = bcm_field_entry_create(unit, group.group, &gr_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    entry1 = gr_entry;
    result = bcm_field_qualify_InPort(unit, gr_entry, in_port, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InPort\n");
        return result;
    }

    result = bcm_field_action_add(unit, gr_entry, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_entry_install(unit, gr_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    /*FG2*/
    BCM_FIELD_QSET_INIT(&qset); 
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_ASET_INIT(&aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    group.group = group2;
    group.priority = priority2;
    group.qset = qset;
    group.aset = aset;
    group.mode = bcmFieldGroupModeAuto;
    group.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
    result = bcm_field_group_config_create(unit, &group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create for group %d\n", group.group);
        return result;
    }

    int ether_type = 0x86dd;
    result = bcm_field_entry_create(unit, group.group, &gr_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    entry2 = gr_entry;
    result = bcm_field_qualify_EtherType(unit, gr_entry, ether_type, 0xFFFF);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_EtherType\n");
        return result;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, out_port);
    result = bcm_field_action_add(unit, gr_entry, bcmFieldActionRedirect, 0, sys_gport);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_entry_install(unit, gr_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;
}


int field_group_direct_extra_drop(int unit, bcm_field_group_t group, int priority)
{
    int result;

    bcm_field_aset_t aset;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t is_equal[2];
    bcm_field_entry_t ent_de;
    bcm_field_group_config_t grp_de;
    bcm_field_group_config_t_init(&grp_de);

    grp_de.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp_de.mode = bcmFieldGroupModeDirectExtraction;
    grp_de.group = group;
    grp_de.priority = priority;

    BCM_FIELD_QSET_INIT(grp_de.qset);
    BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyDstMac);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    result = bcm_field_group_config_create(unit, &grp_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }
    result = bcm_field_group_action_set(unit, grp_de.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }
    result = bcm_field_entry_create(unit, grp_de.group, &ent_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    entry1 = ent_de;
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&is_equal[0]);
    extract.action = bcmFieldActionDrop;
    extract.bias = 0;
    is_equal[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    is_equal[0].bits  = 1;
    is_equal[0].value = 1;
    is_equal[0].qualifier = bcmFieldQualifyConstantOne;
    result = bcm_field_direct_extraction_action_add(unit, ent_de, extract, 1, &is_equal);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }
    result = bcm_field_entry_install(unit, ent_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;

}


int field_group_automode_redirect(int unit, bcm_field_group_t group, int priority)
{
    int result;
    int sys_gport;

    bcm_field_group_config_t group_redir;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    BCM_FIELD_QSET_INIT(&qset); 
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_ASET_INIT(&aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    group_redir.group = group;
    group_redir.priority = priority;
    group_redir.qset = qset;
    group_redir.aset = aset;
    group_redir.mode = bcmFieldGroupModeAuto;
    group_redir.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
    result = bcm_field_group_config_create(unit, &group_redir);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }

    bcm_field_entry_t gr_entry;
    int ether_type = 0x86dd;
    result = bcm_field_entry_create(unit, group_redir.group, &gr_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    entry2 = gr_entry;
    result = bcm_field_qualify_EtherType(unit, gr_entry, ether_type, 0xFFFF);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_EtherType\n");
        return result;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, out_port);
    result = bcm_field_action_add(unit, gr_entry, bcmFieldActionRedirect, 0, sys_gport);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_entry_install(unit, gr_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;
}


/*
 * FEM(drop) vs FES(Redirect)
 * FG1: Drop with direct extract mode
 * FG2: Redirect with bcmFieldGroupModeAuto
 */

int field_group_priority_2(int unit, int priority1, int priority2)
{
    int result;

    group1 = 15;
    group2 = 16;

    result = field_group_direct_extra_drop(unit, group1, priority1);
    if (BCM_E_NONE != result) {
        printf("Error in field_group_direct_extra_drop\n");
        return result;
    }

    result = field_group_automode_redirect(unit, group2, priority2);
    if (BCM_E_NONE != result) {
        printf("Error in field_group_automode_redirect\n");
        return result;
    }

    return result;
}



int field_group_direct_extra_redirect(int unit, bcm_field_group_t group, int priority)
{
    int result;
    
    bcm_field_aset_t aset;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t is_equal[2];
    bcm_field_entry_t ent_de;
    bcm_field_group_config_t grp_de;
    bcm_field_group_config_t_init(&grp_de);

    grp_de.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp_de.mode = bcmFieldGroupModeDirectExtraction;
    grp_de.group = group;
    grp_de.priority = priority;

    BCM_FIELD_QSET_INIT(grp_de.qset);
    BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyDstMac);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    result = bcm_field_group_config_create(unit, &grp_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }
    result = bcm_field_group_action_set(unit, grp_de.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }
    result = bcm_field_entry_create(unit, grp_de.group, &ent_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    entry1 = ent_de;
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&is_equal[0]);
    extract.action = bcmFieldActionRedirect;
    extract.bias = 0x10000;
    is_equal[0].flags = 0;
    is_equal[0].bits  = 8;
    is_equal[0].lsb   = 0;
    is_equal[0].qualifier = bcmFieldQualifyDstMac;
    result = bcm_field_direct_extraction_action_add(unit, ent_de, extract, 1, &is_equal);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }
    result = bcm_field_entry_install(unit, ent_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }


    /*create second entry for no drop together with redirect*/
    result = bcm_field_entry_create(unit, grp_de.group, &ent_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    entry2 = ent_de;
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&is_equal[0]);
    extract.action = bcmFieldActionDrop;
    extract.bias = 0;
    is_equal[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    is_equal[0].bits  = 1;
    is_equal[0].value = 0;                                  /*write drop value = 0 for no drop*/
    is_equal[0].qualifier = bcmFieldQualifyConstantZero;
    result = bcm_field_direct_extraction_action_add(unit, ent_de, extract, 1, &is_equal);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }
    result = bcm_field_entry_install(unit, ent_de);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;
}


int field_group_automode_drop(int unit, bcm_field_group_t group, int priority)
{
    int result;

    bcm_field_group_config_t group_redir;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    BCM_FIELD_QSET_INIT(&qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_INIT(&aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    group_redir.group = group;
    group_redir.priority = priority;
    group_redir.qset = qset;
    group_redir.aset = aset;
    group_redir.mode = bcmFieldGroupModeAuto;
    group_redir.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
    result = bcm_field_group_config_create(unit, &group_redir);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }


    bcm_field_entry_t gr_entry;
    result = bcm_field_entry_create(unit, group_redir.group, &gr_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    entry3 = gr_entry;
    result = bcm_field_qualify_InPort(unit, gr_entry, in_port, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InPort\n");
        return result;
    }
    result = bcm_field_action_add(unit, gr_entry, bcmFieldActionDrop, 0, 14);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_entry_install(unit, gr_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }
    return result;
}



/*
 * FEM(Redirect) vs FES(drop)
 * FG1: Redirect with direct extract mode
 * FG2: Drop with bcmFieldGroupModeAuto
 */

int field_group_priority_3(int unit, int priority1, int priority2)
{
    int result;

    group1 = 25;
    group2 = 26;

    result = field_group_direct_extra_redirect(unit, group1, priority1);
    if (BCM_E_NONE != result) {
        printf("Error in field_group_direct_extra_redirect\n");
        return result;
    }

    result = field_group_automode_drop(unit, group2, priority2);
    if (BCM_E_NONE != result) {
        printf("Error in field_group_automode_drop\n");
        return result;
    }

    return result;
}

int field_group_priority_1_destroy(int unit)
{
    int result;
    group1 = 5;
    group2 = 6;
    result = bcm_field_entry_destroy(unit, entry1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_destroy entry %d\n", entry1);
        return result;
    }

    result = bcm_field_entry_destroy(unit, entry2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_destroy entry %d\n", entry2);
        return result;
    }

    result = bcm_field_group_destroy(unit, group1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_destroy group %d\n", group1);
        return result;
    }

    result = bcm_field_group_destroy(unit, group2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_destroy group %d\n", group2);
        return result;
    }
    return result;
}

int field_group_priority_2_destroy(int unit)
{
    int result;

    group1 = 15;
    group2 = 16;

    result = bcm_field_entry_destroy(unit, entry1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_destroy entry %d\n", entry1);
        return result;
    }

    result = bcm_field_entry_destroy(unit, entry2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_destroy entry %d\n", entry2);
        return result;
    }

    result = bcm_field_group_destroy(unit, group1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_destroy group %d\n", group1);
        return result;
    }

    result = bcm_field_group_destroy(unit, group2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_destroy group %d\n", group2);
        return result;
    }

    return result;

}


int field_group_priority_3_destroy(int unit)
{
    int result;

    group1 = 25;
    group2 = 26;

    result = bcm_field_entry_destroy(unit, entry1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_destroy entry %d\n", entry1);
        return result;
    }

    result = bcm_field_entry_destroy(unit, entry2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_destroy entry %d\n", entry2);
        return result;
    }

    result = bcm_field_entry_destroy(unit, entry3);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_destroy entry %d\n", entry3);
        return result;
    }

    result = bcm_field_group_destroy(unit, group1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_destroy group %d\n", group1);
        return result;
    }

    result = bcm_field_group_destroy(unit, group2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_destroy group %d\n", group2);
        return result;
    }
    return result;
}




