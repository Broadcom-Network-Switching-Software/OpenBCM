/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*  
 *  
 *  This cint contains utility functions for using trill in advanced vlan translation mode. For more information on advanced
 *  vlan translation mode, please refer to cint_advanced_vlan_translation_mode.c
 */


/* In advanced vlan translation mode, default vlan translation actions are not created in the device. Therefore, we create the default	 	 
 * egress actions.  The ingress actions for trill are different, and are used internally when calling bcm_vswitch_port_add, so we	 	 
 * must create them as well.	 	 
 */	 	 
int	 	 
trill_advanced_vlan_translation_actions_init(int unit){	 	 
    int rv;	 	 
 	 	 
    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);	 	 
    if (!advanced_vlan_translation_mode) { /* Nothing to do here */	 	 
        return BCM_E_NONE;	 	 
    }	 	 
 	 	 
    rv = trill_advanced_vlan_translation_ingress_actions_init(unit);	 	 
    if (rv != BCM_E_NONE) {	 	 
        printf("Error, in trill_advanced_vlan_translation_ingress_actions_init\n");	 	 
        return rv;	 	 
    }	 	 
 	 	 
    printf("Done creating default trill ingress actions\n");	 	 
 	 	 
    /* The egress actions for trill are the same as the regular egress actions. */	 	 
    rv = create_default_egress_actions(unit, 1);	 	 
    if (rv != BCM_E_NONE) {	 	 
        printf("Error, in creat_default_eggress_actions\n");	 	 
        return rv;	 	 
    }	 	 
 	 	 
    printf("Done creating default egress actions\n");	 	 
 	 	 
    return rv;	 	 
}

/*
   8 Egress actions are created:
   For each type of packet tag format (untagged, outer tagged, inner tagged or double tagged) we create two profiles:
        REMOVE_TAGS_PUSH_1: to remove all tags, and push a new one
        REMOVE_TAGS_PUSH_2: to remove all tags, and push two new ones
   Then, For each profile, we map the relevant tag to its action.
 
*/
int
trill_advanced_vlan_translation_ingress_actions_init(int unit){
    int rv;
    int action_id;

    /*** Create SINGLE VLAN actions ***/

    /* First SINGLE VLAN action: DT (double tagged) -> SINGLE VLAN */
    action_id = TRILL_SINGLE_VLAN_ACTION + TRILL_DT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION | VLAN_TRANSLATION_TRILL_TPID_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionDelete, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_SINGLE_VLAN_PROFILE + TRILL_DT_OFFSET\n");
        return rv;
    }

    /* Second SINGLE VLAN action: IT (inner tagged) -> SINGLE VLAN */
    action_id = TRILL_SINGLE_VLAN_ACTION + TRILL_IT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION | VLAN_TRANSLATION_TRILL_TPID_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionNone, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_SINGLE_VLAN_PROFILE + TRILL_IT_OFFSET\n");
        return rv;
    }

    /* Third SINGLE VLAN action: UT (untagged) -> SINGLE VLAN */
    action_id = TRILL_SINGLE_VLAN_ACTION + TRILL_UT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION | VLAN_TRANSLATION_TRILL_TPID_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionAdd, bcmVlanActionNone, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_SINGLE_VLAN_PROFILE + TRILL_UT_OFFSET\n");
        return rv;
    }

    /* Fourth SINGLE VLAN action: OT (outer tagged) -> SINGLE VLAN */
    action_id = TRILL_SINGLE_VLAN_ACTION + TRILL_OT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION | VLAN_TRANSLATION_TRILL_TPID_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionNone, TRILL_SINGLE_VLAN_PROFILE, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_SINGLE_VLAN_PROFILE + TRILL_OT_OFFSET\n");
        return rv;
    }

    /**** Create DOUBLE VLAN actions ***/

    /* First DOUBLE VLAN action: DT (double tagged) -> DOUBLE VLAN */
    action_id = TRILL_DOUBLE_VLAN_ACTION + TRILL_DT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionReplace, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_DOUBLE_VLAN_PROFILE + TRILL_DT_OFFSET\n");
        return rv;
    }

    /* Second SINGLE VLAN action: IT (inner tagged) -> SINGLE VLAN */
    action_id = TRILL_DOUBLE_VLAN_ACTION + TRILL_IT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionAdd, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_DOUBLE_VLAN_PROFILE + TRILL_IT_OFFSET\n");
        return rv;
    }

    /* Third SINGLE VLAN action: UT (untagged) -> DOUBLE VLAN */
    action_id = TRILL_DOUBLE_VLAN_ACTION + TRILL_UT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionAdd, bcmVlanActionAdd, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_DOUBLE_VLAN_PROFILE + TRILL_UT_OFFSET\n");
        return rv;
    }

    /* Fourth DOUBLE VLAN action: OT (outer tagged) -> SINGLE VLAN */
    action_id = TRILL_DOUBLE_VLAN_ACTION + TRILL_OT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionAdd, TRILL_DOUBLE_VLAN_PROFILE, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_DOUBLE_VLAN_PROFILE + TRILL_OT_OFFSET\n");
        return rv;
    }

    /**** Create TTS VLAN actions ***/

    /* First TTS VLAN action: DT (double tagged) -> DOUBLE VLAN */
    action_id = TRILL_TTS_VLAN_ACTION + TRILL_DT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionReplace, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_SINGLE_VLAN_PROFILE + TRILL_DT_OFFSET\n");
        return rv;
    }

    /* Second TTS VLAN action: IT (inner tagged) -> DOUBLE VLAN */
    action_id = TRILL_TTS_VLAN_ACTION + TRILL_IT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionCopy, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_SINGLE_VLAN_PROFILE + TRILL_IT_OFFSET\n");
        return rv;
    }

    /* Third TTS VLAN action: UT (untagged) -> SINGLE VLAN */
    action_id = TRILL_TTS_VLAN_ACTION + TRILL_UT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION | VLAN_TRANSLATION_TRILL_TPID_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionAdd, bcmVlanActionNone, -1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_SINGLE_VLAN_PROFILE + TRILL_UT_OFFSET\n");
        return rv;
    }

    /* Fouth TTS VLAN action: OT (outer tagged) -> DOUBLE VLAN */
    action_id = TRILL_TTS_VLAN_ACTION + TRILL_OT_OFFSET;
    action_id |= VLAN_TRANSLATION_TRILL_ACTION;
    rv = vlan_translation_create_mapping(unit, action_id, bcmVlanActionReplace, bcmVlanActionCopy, TRILL_TTS_VLAN_PROFILE, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_create_mapping, TRILL_DOUBLE_VLAN_PROFILE + TRILL_OT_OFFSET\n");
        return rv;
    }

    return rv;
}

/* In normal vlan translation mode, calling bcm_vlan_port_create and bcm_vswitch_port_create (with trill) internally creates mapping between vlan
   ports and certain vlan translation actions. In advanced vlan translation mode, these mappings are not created, and must be created by calling
   additional APIs.
 
   Arguments:
        vsi         - vsi used in bcm_vswitch_port_create
        egress_vlan - the one used in bcm_vlan_port_create
        port        - the vlan_port_id gained after calling bcm_vlan_port_create
 */
int
trill_advanced_vlan_translation_map_local_vsi_to_global_vsi(int unit, int vsi, int egress_vlan, int port, int trill_mode){
    int rv;
    int ingress_edit_profile;
    int egress_edit_profile = EGRESS_REMOVE_TAGS_PUSH_1_PROFILE; /* This profile is always used. */
    int double_vlan = 0;
    int low_vid;
    bcm_trill_vpn_config_t trill_config;

    if (!advanced_vlan_translation_mode || (trill_mode !=2)) { /* Nothing to do here */
        return BCM_E_NONE;
    }

    bcm_trill_vpn_config_t_init(&trill_config);
    rv = bcm_trill_vpn_get(unit, vsi, &trill_config);

    if (trill_config.flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) {
        low_vid = trill_config.high_vid;
        ingress_edit_profile = TRILL_TTS_VLAN_PROFILE;
    } else if (trill_config.low_vid != BCM_VLAN_INVALID) {  /* Double vlan */
        low_vid = trill_config.low_vid;
        ingress_edit_profile = TRILL_DOUBLE_VLAN_PROFILE;
    } else {    /* Single vlan */
        low_vid = trill_config.high_vid;
        ingress_edit_profile = TRILL_SINGLE_VLAN_PROFILE;
    }


    rv = vlan_port_translation_set(unit, trill_config.high_vid, low_vid, port, ingress_edit_profile, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set, ingress profile\n");
        return rv;
    }
    
    rv = vlan_port_translation_set(unit, egress_vlan, low_vid, port, egress_edit_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set, egress profile\n");
        return rv;
    }

    return rv;
}


