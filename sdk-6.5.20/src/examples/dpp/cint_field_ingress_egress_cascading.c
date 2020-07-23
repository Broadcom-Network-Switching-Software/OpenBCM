/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/* 
 *
 * Main function:
 * --------------
 * cascade_main(unit, group_ing, group_egr, group_ing_priority, group_egr_priority, in_port, in_port1, method) 
 *     Main function to create one field group at Ingress stage
 *     and another one at Egress stage.
 * 
 * L2 config function:
 * ----------------
 * cascade_vlan_and_ports_config(unit, vsi, in_port, in_port1, out_port)
 *     Creating a VSI and attaching one ingress and one egress port to it.
 * 
 * Cleanup function:
 * ----------------
 * cascade_field_destroy( unit, group_ing, group_egr)
 *     Destroying the created groups and entries
 */



/* ***************************************************************** */
/* 
 * cascade_field_group_ingress(): 
 *  
 * 1. Create the first field group in Ingress Field Processor 
 * 2. Set the QSET. 
 * 3. Set the ASET. 
 * Variable "method" meaning:
 *      - if it is 1: User-Header example
 *      - if it is 2: Stacking Extention example
 *      - if it is 3: Learn Extension example
 *      - if it is 4: Combination of User-Header and Stacking Extension
 */
/* ***************************************************************** */
int cascade_field_group_ingress(int unit, bcm_field_group_t group_ing, int group_ing_priority, bcm_port_t in_port, bcm_port_t in_port1, int method) {

    int rv = 0;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_entry_t ent1;
    bcm_field_group_config_t ing_group;

    /* 
   * Define the first Field Group.
   */
    bcm_field_group_config_t_init(&ing_group);
    ing_group.group = group_ing;
    ing_group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    ing_group.mode = bcmFieldGroupModeAuto;
    ing_group.priority = group_ing_priority;

    /* 
    * Define the QSET. 
    */
    BCM_FIELD_QSET_INIT(ing_group.qset);
    BCM_FIELD_QSET_ADD(ing_group.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(ing_group.qset, bcmFieldQualifyInPort);

    /* Define the ASET */
    BCM_FIELD_ASET_INIT(aset);
    if (method == 1) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);
    }else if (method == 2) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStackingRouteNew);
    }else if (method == 3) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionFabricHeaderSet);
    }else if (method == 4) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStackingRouteNew);
    }

    /* Create the Ingress Field group */
    rv = bcm_field_group_config_create(unit, &ing_group);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_config_create returned %d for group %d\n", rv, ing_group.group);
        return rv;
    }
 
    /* Attach the action set to the field group*/
    rv = bcm_field_group_action_set(unit, ing_group.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_action_set returned %d for group %d\n", rv, ing_group.group);
        return rv;
    }

    /* Create an entry to the field group*/
    rv = bcm_field_entry_create(unit, ing_group.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_entry_create returned %d\n", rv);
        return rv;
    }

    /* Create an entry to the field group*/
    rv = bcm_field_entry_create(unit, ing_group.group, &ent1);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_entry_create returned %d\n", rv);
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, ent, in_port, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_InPort returned %d\n", rv);
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, ent1, in_port1, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_InPort returned %d\n", rv);
        return rv;
    }

    if (method == 1) {
        rv = bcm_field_action_add(unit, ent, bcmFieldActionClassDestSet, 5, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add User-Header\n");
            return rv;
        }
        rv = bcm_field_action_add(unit, ent1, bcmFieldActionClassDestSet, 66, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add User-Header\n");
            return rv;
        }
    }else if (method == 2) {
        rv = bcm_field_action_add(unit, ent, bcmFieldActionStackingRouteNew, 5, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add Stacking Extension\n");
            return rv;
        }
        rv = bcm_field_action_add(unit, ent1, bcmFieldActionStackingRouteNew, 66, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add Stacking Extension\n");
            return rv;
        }
    }else if (method == 3) {
        rv = bcm_field_action_add(unit, ent, bcmFieldActionFabricHeaderSet, bcmFieldFabricHeaderEthernetLearn, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add Learn Extension\n");
            return rv;
        }
    }else if (method == 4) {
        rv = bcm_field_action_add(unit, ent, bcmFieldActionClassDestSet, 5, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add User-Header\n");
            return rv;
        }

        rv = bcm_field_action_add(unit, ent, bcmFieldActionStackingRouteNew, 5, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add Stacking Extension\n");
            return rv;
        }

        rv = bcm_field_action_add(unit, ent1, bcmFieldActionClassDestSet, 6, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add User-Header\n");
            return rv;
        }

        rv = bcm_field_action_add(unit, ent1, bcmFieldActionStackingRouteNew, 6, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_action_add Stacking Extension\n");
            return rv;
        }
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_install%d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_install%d\n", rv);
        return rv;
    }

    return rv;
}


/* ***************************************************************** */
/* 
 * cascade_field_group_egress(): 
 *  
 * 1. Create Second field group in Egress Field Processor. 
 * 2. Set the QSET to be bcmFieldQualifyDstClassField.
 * 3. Set the action to be bcmFieldActionDrop. 
 * Variable "method" meaning:
 *      - if it is 1: User-Header example
 *      - if it is 2: Stacking Extention example
 *      - if it is 3: Learn Extension example
 *      - if it is 4: Combination of User-Header and Stacking Extension 
 */
/* ***************************************************************** */
int cascade_field_group_egress(int unit, bcm_field_group_t group_egr, int group_egr_priority, int method) {

    int rv = 0;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t egr_group;

    /* 
   * Define the first Field Group.
   */
    bcm_field_group_config_t_init(&egr_group);
    egr_group.group = group_egr;
    egr_group.flags = BCM_FIELD_GROUP_CREATE_WITH_ID;
    egr_group.priority = group_egr_priority;

    /* 
    * Define the QSET. 
    */
    BCM_FIELD_QSET_INIT(egr_group.qset);
    BCM_FIELD_QSET_ADD(egr_group.qset, bcmFieldQualifyStageEgress);
    if (method == 1) {
        BCM_FIELD_QSET_ADD(egr_group.qset, bcmFieldQualifyDstClassField);
    }else if (method == 2) {
        BCM_FIELD_QSET_ADD(egr_group.qset, bcmFieldQualifyStackingRoute);
    }else if (method == 3) {
        BCM_FIELD_QSET_ADD(egr_group.qset, bcmFieldQualifyLearnExtension);
    }else if (method == 4) {
        BCM_FIELD_QSET_ADD(egr_group.qset, bcmFieldQualifyDstClassField);
        BCM_FIELD_QSET_ADD(egr_group.qset, bcmFieldQualifyStackingRoute);
    }

    /* Define the ASET */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    /* Create the Egress Field group*/
    rv = bcm_field_group_config_create(unit, &egr_group);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_config_create returned %d for group %d\n", rv, egr_group.group);
        return rv;
    }

    /* Attach the action set to the field group*/
    rv = bcm_field_group_action_set(unit, egr_group.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_action_set returned %d for group %d\n", rv, egr_group.group);
        return rv;
    }

    /* Create an entry to the field group*/
    rv = bcm_field_entry_create(unit, egr_group.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_entry_create returned %d\n", rv);
        return rv;
    }    

    if (method == 1) {
        rv = bcm_field_qualify_DstClassField(unit, ent, 5, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_InPort returned %d\n", rv);
            return rv;
        }
    }else if (method == 2) {
        rv = bcm_field_qualify_StackingRoute(unit, ent, 5, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_InPort returned %d\n", rv);
            return rv;
        }
    }else if (method == 3) {
        rv = bcm_field_qualify_LearnExtension(unit, ent, 1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_InPort returned %d\n", rv);
            return rv;
        }
    }else if (method == 4) {
        rv = bcm_field_qualify_DstClassField(unit, ent, 5, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_InPort returned %d\n", rv);
            return rv;
        }

        rv = bcm_field_qualify_StackingRoute(unit, ent, 5, 0xff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_InPort returned %d\n", rv);
            return rv;
        }
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add\n");
        return rv;
    }
    
    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_install returned %d\n", rv);
        return rv;
    }

    return rv;
}


/* Main function
 *  
 *   Variable "method" meaning:
 *      - if it is 1: User-Header example
 *      - if it is 2: Stacking Extention example
 *      - if it is 3: Learn Extension example
 *      - if it is 4: Combination of User-Header and Stacking Extension
 */
int cascade_main(int unit, bcm_field_group_t group_ing, bcm_field_group_t group_egr, int group_ing_priority, int group_egr_priority, bcm_port_t in_port, bcm_port_t in_port1, int method) {

    int rv = 0;

    rv = cascade_field_group_ingress(unit, group_ing, group_ing_priority, in_port, in_port1, method);
    if (rv != BCM_E_NONE) {
        printf("Error in cascade_field_group_ingress returned %d\n", rv);
        return rv;
    }

    rv = cascade_field_group_egress(unit, group_egr, group_egr_priority, method);
    if (rv != BCM_E_NONE) {
        printf("Error in cascade_field_group_egress returned %d\n", rv);
        return rv;
    }

    return rv;
}


int 
cascade_vlan_and_ports_config(int unit, int vsi, int in_port, int in_port1, int out_port)
{
    int rv;

    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vsi, in_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, vsi);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vsi, in_port1, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port1, vsi);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vsi, out_port, BCM_VLAN_GPORT_ADD_EGRESS_ONLY);
    if (rv) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, vsi);
        return rv;
    }

  
    return rv;
}


/* ***************************************************************** */
/*
 *  Destroy configuration of this cint
 */
/* ***************************************************************** */
int cascade_field_destroy(int unit, bcm_field_group_t group_ing, bcm_field_group_t group_egr) {

    int rv = 0;

    /*destroy group and entry at ingress stage*/
	rv = bcm_field_group_flush(unit, group_ing);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_flush at ingress stage, returned %d\n", rv);
        return rv;
    }

    /*destroy group and entry at egress stage*/
	rv = bcm_field_group_flush(unit, group_egr);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_group_flush at egress stage, returned %d\n", rv);
        return rv;
    }

    return rv;
}
