/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:field_group_ipv6_tunnel_presel_dvapi_example
 * Purpose: Example test bcmFieldHeaderFormatIp4Ip6.
 *
 * Fuctions:
 * Main function:
 *      field_group_ipv6_tunnel_presel_dvapi_example()
 *
 * Cleanup function:
 *      field_group_ipv6_tunnel_presel_destroy()
 */

bcm_field_group_t group;

int field_group_ipv6_tunnel_presel_dvapi_example(int unit, bcm_field_presel_t presel_id, int group_priority, bcm_gport_t in_port) {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_t grp;
    int presel;
    bcm_field_presel_set_t psst;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;

    /* Create a presel entity for MPLS with three labels */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }
    rv = bcm_field_qualify_HeaderFormat(unit, presel_id | presel_flags, bcmFieldHeaderFormatIp4Ip6);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_HeaderFormat\n");
        return rv;
    }

    BCM_FIELD_PRESEL_INIT(psst);
    BCM_FIELD_PRESEL_ADD(psst, presel_id);

    /*Configure PMF*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    rv = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != rv) {
        bcm_field_group_destroy(unit, group);
        printf("Error in bcm_field_group_create\n");
        return rv;
    }

    rv = bcm_field_group_presel_set(unit, grp, &psst);
    if (BCM_E_NONE != rv) {
        bcm_field_group_destroy(unit, group);
        printf("Error in bcm_field_group_presel_set\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != rv) {
        bcm_field_group_destroy(unit, group);
        printf("Error in bcm_field_group_action_set\n");
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != rv) {
        bcm_field_group_destroy(unit, group);
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit,ent,in_port,0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_SrcPort\n");
        bcm_field_entry_destroy(unit, ent);
        bcm_field_group_destroy(unit, group);
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_action_add\n");
        bcm_field_entry_destroy(unit, ent);
        bcm_field_group_destroy(unit, group);
        return rv;
    }

    rv = bcm_field_group_install(unit, grp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_install\n");
        bcm_field_entry_destroy(unit, ent);
        bcm_field_group_destroy(unit, group);
        return rv;
    }

    group = grp;
    return rv;
}

/*
Function to destroy the configured field group and entry
*/
int field_group_ipv6_tunnel_presel_destroy(int unit) {
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
