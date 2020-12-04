/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_custom_ethertype_test.c
 * Purpose: Example stamp custom Ethernet Type.
 *
 * Fuctions:
 * Main function:
 *      custom_ethertype_example_dvapi()
 *
 * Cleanup function:
 *      custom_ethertype_example_dvapi_clean_up()
 */

int custom_forwarding_type_entry = 0;
int custom_forwarding_type_group_id = 0;

int field_custom_forwarding_type(int unit, bcm_field_ForwardingType_t forwarding_type) {
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t grp;
    bcm_field_entry_t ent;
    bcm_field_action_core_config_t config;
    int rv = BCM_E_NONE;

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstIp);

    grp.priority = 2;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &grp));

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionForwardingTypeNew);

    BCM_IF_ERROR_RETURN(bcm_field_group_action_set(unit, grp.group, aset));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, grp.group, &ent));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstIp(unit, ent, 0xa00ff01, 0xFFFFFFFF));

    config.param0 = forwarding_type;
    config.param1 = 2;
    config.param2 = 0;
    rv = bcm_field_action_config_add(unit, ent, bcmFieldActionForwardingTypeNew, 1, &config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_action_config_add\n");
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_install\n");
    }

    custom_forwarding_type_entry = ent;
    custom_forwarding_type_group_id = grp.group;

    return rv;
}

int custom_ethertype_example_dvapi(int unit, bcm_switch_control_t type, int EtherType, bcm_field_ForwardingType_t forwarding_type) {
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(field_custom_forwarding_type(unit, forwarding_type));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, type, EtherType));

    return rv;
}

int custom_ethertype_example_dvapi_clean_up(int unit) {
    bcm_field_entry_destroy(unit, custom_forwarding_type_entry);
    bcm_field_group_destroy(unit, custom_forwarding_type_group_id);

    return BCM_E_NONE;
}

