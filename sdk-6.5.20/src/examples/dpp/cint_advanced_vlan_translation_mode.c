/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * set following soc properties
 * # Specify the "bcm886xx_vlan_translate_mode" mode. 0: disabled, 1: enabled.
 * bcm886xx_vlan_translate_mode=1
 *  
 * Example 1:  
 * The cint creates a simple bridging application and ingress/egress vlan editing configuration.
 * In this example, we would like the TPID to be transparent in a "replace" vlan translation.
 * Meaning, for incoming packet with TPID 0x8100 and VID 10, the outgoing packet should be with TPID 0x8100 and VID 20.
 * For incoming packet with TPID 0x9100 and VID 10, the outgoing packet should be with TPID 0x9100 and VID 20.
 *
 *
 * run:
 * cd ../../../../src/examples/dpp
 * cint cint_advanced_vlan_translation_mode.c
 * cint cint_port_tpid.c
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint
 * int unit = 0;
 * ive_translation_main(unit,port1,port2); OR eve_translation_main(unit,port1,port2);
 *
 *
 * Main configuration:
 * Set both ports with 2 TPIDs (0x8100, 0x9100).
 * Set LIF edit profile.
 * Set different tag format per TPID.
 * Set different action class per tag format.
 * Set different action per action class (derived from edit profile and tag format)
 *
 *
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1 
 *      Vlan tag: vlan-id 10, vlan tag type 0x8100
 * from <port1>
 *       the packet will arrive at port 2, vlan-id 20, vlan tag type 0x8100
 * 
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1 
 *      Vlan tag: vlan-id 10, vlan tag type 0x9100
 * from <port1>
 *       the packet will arrive at port 2, vlan-id 20, vlan tag type 0x9100
 *  
 *  
 * Example 2: 
 * Ingress VLAN translation: In case of no match in VTT SEM L2 LIF lookup a default action command per port introduced. 
 * User can set different actions per port by setting vlan_translation_action_id field in bcm_port_tpid_class_set.
 *  
 * run: 
 * cd ../../../../src/examples/dpp
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_port_tpid.c 
 * cint
 * int unit = 0; 
 * ive_main_port_default_run(unit, port1,port2,port3, port4); 
 *  
 * Main configuration: 
 * Create action command 
 * Set port TPIDs 
 * Set default action command per port. 
 * Associate port TPIDs settings to default action command 
 * Set action command properties 
 *  
 * run packet:
 *      ethernet header with DA 00:00:00:00:00:11 
 *      Vlan tag: vlan-id 10, vlan tag type 0x8100
 * from <port1>
 *       the packet will arrive at port 4, untag
 *  
 * run packet:
 *      ethernet header with DA 00:00:00:00:00:12 
 *      Vlan tag: vlan-id 10, vlan tag type 0x8100
 * from <port2>
 *       the packet will arrive at port 4, vlan-id 10, inner-vlan-id 10
 *  
 * run packet:
 *      ethernet header with DA 00:00:00:00:00:13 
 *      Vlan tag: vlan-id 10, vlan tag type 0x8100
 * from <port3>
 *       the packet will arrive at port 4, vlan-id 10
 *  
 *  
 * Example 3:  
 * Default egress translation actions are performed when there is no match found for existing out-LIF. 
 * The user can change the default egress translation actions by creating a default out-LIF per port. 
 * In this example, we first test the default actions: add vlan (new vlan = default vsi) for untagged packets. NOP for single tagged packets.
 * Then we change the default actions: NOP for untagged packets. Replcae outer vlan and add inner vlan for single tagged packets.
 * Then we create a default out-LIF and configure new default actions: add outer vlan (30) for both untagged and single tagged packets.  
 *  
 *  
 * run:
 * cd ../../../../src/examples/dpp
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_port_tpid.c 
 * cint
 * int unit = 0;
 * eve_default_translation_main(int unit, int port1, int port2); // set default configuration
 * eve_default_translation_change(int unit); // change default translation actions configuration
 * default_vlan_port_set(int unit, int port); // set new default actions 
 *  
 * 
 * Main configuration: 
 * Set both ports TPIDs.
 * Change default actions.
 * Set default out-LIF. 
 * Set LIF edit profile. 
 * Set default action command. 
 * Set action class.   
 *  
 *  
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1, untagged 
 * from <port1>
 *       the packet will arrive at port 2, vlan-id 30, vlan tag type 0x8100
 * 
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1 
 *      Vlan tag: vlan-id 10, vlan tag type 0x8100
 * from <port1>
 *       the packet will arrive at port 2, outer-vlan-id 30, vlan tag type 0x8100
 *                                         inner-vlan-id 10, vlan tag type 0x8100
 *  
 *  
 * Example 4: 
 * Ingress priority and non priority packets handling. 
 * The user can change the default handling of incoming priority and non priority packets by LLVP configuration. 
 * In this example, one port's behavior is default (incoming priority and non priority packets are treated the same). 
 * The second port is configured to pass non priority packets and drop priority packets. 
 * The third port is configured to set different tag format for priority packets and untagged packets, 
 * so they can have diferent translation actions. 
 *  
 *  
 * run:
 * cd ../../../../src/examples/dpp
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_port_tpid.c 
 * cint
 * int unit = 0;
 * ive_priority_tags_main(int unit, int port1, int port2, int port3, int port4); 
 *  
 * 
 * Main configuration: 
 * Set ports TPIDs according to desired behavior 
 *  
 *  
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1
 *      Vlan tag: vlan-id 1, vlan tag type 0x8100
 * from <port1>
 *       the packet will arrive at port 4, vlan-id 1, vlan tag type 0x8100
 * 
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1 
 *      Vlan tag: vlan-id 0, vlan tag type 0x8100
 * from <port1>
 *       the packet will arrive at port 4, vlan-id 0, vlan tag type 0x8100
 *  
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1
 *      Vlan tag: vlan-id 1, vlan tag type 0x8100
 * from <port2>
 *       the packet will arrive at port 4, vlan-id 1, vlan tag type 0x8100
 * 
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1 
 *      Vlan tag: vlan-id 0, vlan tag type 0x8100
 * from <port2>
 *       packet is dropped.
 *  
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1, untagged
 * from <port1>
 *       the packet will arrive at port 4, vlan-id 40, vlan tag type 0x8100
 * 
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1 
 *      Vlan tag: vlan-id 0, vlan tag type 0x8100
 * from <port1>
 *       the packet will arrive at port 4, untagged
 *   
 *
 *
 * Example 5: 
 * Egress priority and non priority packets handling. 
 * The user can change the default handling of incoming priority and non priority packets by LLVP configuration. 
 * In this example, the RX port is configured to set different tag format for priority packets and untagged packets, 
 * so they can have diferent egress translation actions. 
 *  
 *  
 * run:
 * cd ../../../../src/examples/dpp
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_port_tpid.c 
 * cint
 * int unit = 0;
 * eve_priority_tags_main(int unit, int port1, int port2); 
 *  
 * 
 * Main configuration: 
 * Set ports TPIDs according to desired behavior
 * Set default out-LIF. 
 * Set LIF edit profile. 
 * Set default action command. 
 * Set action class.  
 *
 *
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1, untagged
 * from <port1>
 *       the packet will arrive at port 2, vlan-id 50, vlan tag type 0x8100
 * 
 * run packet:
 *      ethernet header with DA 00:11:22:33:44:55, SA 1 
 *      Vlan tag: vlan-id 0, vlan tag type 0x8100
 * from <port1>
 *       the packet will arrive at port 4, untagged
 */

int verbose = 1;
int ingress_qos_map_id = 0;
int egress_qos_map_id = 0;

int advanced_vlan_translation_mode = 0;


/* Set configuration for ingress vlan editing */
int
ive_translation_main(int unit, int port1, int port2) {
    bcm_gport_t vlan_port;
    bcm_vlan_t match_vid = 10;
    bcm_vlan_t new_vid = 20;
    int action_id_1=5, action_id_2=6;
    bcm_port_tpid_class_t port_tpid_class;
    int rv;

    /* set class for both ports */
    rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port1);
        return rv;
    }
    
    rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port2);
        return rv;
    }

    /* set TPIDs for port1 */
    /* JR2: use default application reference to configure port TPID setting */
    if (!is_device_or_above(unit, JERICHO2)){
        port_tpid_init(port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }
        
        /* set TPIDs for port2 */
        port_tpid_init(port2, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_2\n");
            print rv;
            return rv;
        }
    }

    /* create a vlan port:
       egress_vlan (out vlan) = match_vlan (in vlan) */
    if (!is_device_or_above(unit, JERICHO2)){
        rv = vlan_port_create(unit, port1, &vlan_port, match_vid, BCM_VLAN_PORT_MATCH_PORT_VLAN);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_create\n");
            return rv;
        }
    } else {
        bcm_vlan_port_t vp;
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = port1;
        vp.match_vlan = match_vid;
        vp.vsi = match_vid;
        vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n"); 
            print rv;
            return rv;
        }
        vlan_port = vp.vlan_port_id;

        /* Add classification for CTAG: 0x8100 to the same LIF as STAG:0x9100 */
        bcm_port_match_info_t match_info;
        bcm_port_match_info_t_init(&match_info);
        match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
        match_info.port = port1;
        match_info.match = BCM_PORT_MATCH_PORT_CVLAN;
        match_info.match_vlan = match_vid;
        rv = bcm_port_match_add(unit, vlan_port, &match_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_port_match_add\n", rv);
            return rv;
        }
    }
    rv = bcm_vlan_gport_add(unit, match_vid, port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_port_match_add\n", rv);
        return rv;
    }

    /* set port translation info */
    rv = vlan_port_translation_set(unit, new_vid, new_vid, vlan_port, 2, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    /* set editing actions */
    rv = vlan_translate_action_set(unit, action_id_1, action_id_2, 1, bcmVlanActionReplace, bcmVlanActionNone);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translate_action_set\n");
        return rv;
    }

    /* set action class in IVEC map */
    rv = vlan_translate_action_class_set(unit, action_id_1, action_id_2, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translate_action_class_set\n");
        return rv;
    }

    /* set static entry in MAC table */
    rv = l2_addr_add(unit, port2, match_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    /* Create VSI */
    rv = bcm_vlan_create(unit, match_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create\n");
        return rv;
    }

    return rv;
}

/* Set configuration for ingress vlan editing port default settings */
int
ive_main_port_default_run(int unit, int port1, int port2, int port3, int port4) {
    int rv;
    bcm_mac_t mac1  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    bcm_mac_t mac2  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x12};
    bcm_mac_t mac3  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x13};
    bcm_l2_addr_t l2_addr;    
    int vid = 10;
    int action_id_1 = 20;
    int action_id_2 = 21;
    int action_id_3 = 22;
    int flags = BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID;  

    /* this case should run in normal default translation model if device is jericho */
    if (is_device_or_above(unit, JERICHO)) {
        rv = bcm_switch_control_set(unit,bcmSwitchPortVlanTranslationAdvanced,0);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_switch_control_set  rv %d\n", rv);
            return rv;
        }
    }
    
    /* Create new Action IDs */
    rv = bcm_vlan_translate_action_id_create( unit, flags, &action_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    rv = bcm_vlan_translate_action_id_create( unit, flags, &action_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    rv = bcm_vlan_translate_action_id_create( unit, flags, &action_id_3);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /* Set Port TPIDs */
    port_tpid_init(port1,1,1);    
    /* Set Port Default VLAN-Action-CMD 1 */
    port_tpid_info1.vlan_transation_profile_id = action_id_1;
    printf("action_id_1 %d \n",action_id_1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }

    port_tpid_init(port2,1,1);
    /* Set Port Default VLAN-Action-CMD 2 */
    port_tpid_info1.vlan_transation_profile_id = action_id_2;
    printf("action_id_1 %d \n",action_id_2);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }

    port_tpid_init(port3,1,1);
    /* Set Port Default VLAN-Action-CMD 3 */
    port_tpid_info1.vlan_transation_profile_id = action_id_3;
    printf("action_id_1 %d \n",action_id_3);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }

    /* 
     * Set port1 Action command: 
     * Remove tag i.e. get out untagged. 
     * At the egress, default action command add VSI (=VLAN) 
     */
    rv = vlan_translation_action_add(unit, action_id_1, BCM_VLAN_ACTION_SET_INGRESS, bcmVlanActionDelete, bcmVlanActionNone, 0x8100, 0x9100);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translation_action_add\n");
        return rv;
    }

    /*
     * Set port2 Action command: 
     * Add tag (Initial-VID). 
     */
    rv = vlan_translation_action_add(unit, action_id_2, BCM_VLAN_ACTION_SET_INGRESS, bcmVlanActionAdd, bcmVlanActionNone, 0x8100, 0x9100);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translation_action_add\n");
        return rv;
    }
    
    /*  
     * Set port3 Action command: 
     * NOP 
     */
    rv = vlan_translation_action_add(unit, action_id_3, BCM_VLAN_ACTION_SET_INGRESS, bcmVlanActionNone, bcmVlanActionNone, 0x8100, 0x9100);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translation_action_add\n");
        return rv;
    }
    
    /* set static entry in MAC table */
    bcm_l2_addr_t_init(&l2_addr, mac1, vid);
    l2_addr.port = port4;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2_addr, mac2, vid);   
    l2_addr.port = port4;
     
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2_addr, mac3, vid);  
    l2_addr.port = port4;  
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

/* Set configuration for egress vlan editing */
int
eve_translation_main(int unit, int port1, int port2) {
    bcm_gport_t in_vlan_port, out_vlan_port;
    bcm_vlan_port_t vp;
    bcm_vlan_t match_vid = 10;
    bcm_vlan_t new_vid = 20;
    int action_id_1=5, action_id_2=6;
    int rv;

    /* set class for both ports */
    rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port1);
        return rv;
    }
    
    rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port2);
        return rv;
    }

    /* set TPIDs for port1 */
    /* JR2: use default application reference to configure port TPID setting */
    if (!is_device_or_above(unit, JERICHO2)){
        port_tpid_init(port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }
        
        /* set TPIDs for port2 */
        port_tpid_init(port2, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_2\n");
            print rv;
            return rv;
        }
    }

    /* create a vlan port (in-Lif):
       egress_vlan (out vlan) = match_vlan (in vlan) */
    if (!is_device_or_above(unit, JERICHO2)){
        rv = vlan_port_create(unit, port1, &in_vlan_port, match_vid, BCM_VLAN_PORT_MATCH_PORT_VLAN);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_create\n");
            return rv;
        }
    } else {
        bcm_vlan_port_t vp;
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = port1;
        vp.match_vlan = match_vid;
        vp.vsi = match_vid;
        vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n"); 
            print rv;
            return rv;
        }
        in_vlan_port = vp.vlan_port_id;

        bcm_port_match_info_t match_info;
        bcm_port_match_info_t_init(&match_info);
        match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
        match_info.port = port1;
        match_info.match = BCM_PORT_MATCH_PORT_CVLAN;
        match_info.match_vlan = match_vid;
        rv = bcm_port_match_add(unit, in_vlan_port, &match_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_port_match_add\n", rv);
            return rv;
        }
        rv = bcm_vlan_gport_add(unit, match_vid, port1, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_vlan_gport_add\n", rv);
            return rv;
        }
    }

    /* create a vlan port (out-Lif):
    egress_vlan (out vlan) = match_vlan (in vlan) */
    if (!is_device_or_above(unit, JERICHO2)){
        rv = vlan_port_create(unit, port2, &out_vlan_port, match_vid, BCM_VLAN_PORT_MATCH_PORT_VLAN);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_create\n");
            return rv;
        } 
    } else {
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = port2;
        vp.vsi = match_vid;
        vp.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n"); 
            print rv;
            return rv;
        }
        out_vlan_port = vp.vlan_port_id;
    }

    /* set NOP for ingress LIF */
    rv = vlan_port_translation_set(unit, match_vid, match_vid, in_vlan_port, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    /* set edit profile for egress LIF */
    rv = vlan_port_translation_set(unit, new_vid, new_vid, out_vlan_port, 2, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    /* set editing actions*/
    rv = vlan_translate_action_set(unit, action_id_1, action_id_2, 0, bcmVlanActionReplace, bcmVlanActionNone);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translate_action_set\n");
        return rv;
    }

    /* set action class in IVEC map */
    rv = vlan_translate_action_class_set(unit, action_id_1, action_id_2, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translate_action_class_set\n");
        return rv;
    }

    /* set static entry in MAC table */
    rv = l2_addr_add(unit, port2, match_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    /* Create VSI */
    rv = bcm_vlan_create(unit, match_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create\n");
        return rv;
    }

    return rv;
}

/* Set configuration for default egress vlan editing */
int
eve_default_translation_main(int unit, int port1, int port2) {
    bcm_gport_t in_vlan_port, out_vlan_port;
    bcm_vlan_port_t vp;
    bcm_vlan_t match_vid = 10;
    bcm_vlan_t default_vsi = 1;
    bcm_vlan_t default_vid = 30;
    int action_id_1=5, action_id_2=6;
    int rv;

    /* set class for both ports */
    rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port1);
        return rv;
    }
    
    rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port2);
        return rv;
    }

    /* set TPIDs for port1 */
    /* JR2: use default application reference to configure port TPID setting */
    if (!is_device_or_above(unit, JERICHO2)){
        port_tpid_init(port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }
        
        /* set TPIDs for port2 */
        port_tpid_init(port2, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_2\n");
            print rv;
            return rv;
        }
    }

    /* create a vlan port (in-Lif):
       egress_vlan (out vlan) = match_vlan (in vlan) */
    if (!is_device_or_above(unit, JERICHO2)){
        rv = vlan_port_create(unit, port1, &in_vlan_port, match_vid, BCM_VLAN_PORT_MATCH_PORT_VLAN);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_create\n");
            return rv;
        }
    } else {
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = port1;
        vp.match_vlan = match_vid;
        vp.vsi = match_vid;
        vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n"); 
            print rv;
            return rv;
        }
        in_vlan_port = vp.vlan_port_id;
    }
    rv = bcm_vlan_gport_add(unit, match_vid, port1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_gport_add\n");
        print rv;
        return rv;
    }

    /* set NOP for ingress LIF */
    rv = vlan_port_translation_set(unit, match_vid, match_vid, in_vlan_port, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    /* set static entry in MAC table for default VSI */
    rv = l2_addr_add(unit, port2, default_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    /* set static entry in MAC table for match vid*/
    rv = l2_addr_add(unit, port2, match_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    return rv;
}

/* Change configuration for default egress vlan editing actions */
int
eve_default_translation_change(int unit) {
    bcm_vlan_action_set_t action;
    int default_action_id_1 = 0, default_action_id_2 = 1;
    int rv;

    /* get the default action */
    rv = bcm_vlan_translate_action_id_get( unit, 
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           default_action_id_1,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_get \n");
        return rv;
    }  

    /* Change default action */
    action.dt_outer = bcmVlanActionNone;
    action.dt_outer_pkt_prio = bcmVlanActionNone;

    /* set the new default action */
    rv = bcm_vlan_translate_action_id_set( unit, 
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           default_action_id_1,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set \n");
        return rv;
    }

    /* get the default action */
    rv = bcm_vlan_translate_action_id_get( unit, 
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           default_action_id_2,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_get \n");
        return rv;
    } 

    /* Change default action */
    action.dt_outer = bcmVlanActionMappedReplace;
    action.dt_inner = bcmVlanActionMappedAdd;
    /* on JR2, have to configure tpid for action replace/add */
    action.outer_tpid = 0x9100;
    action.inner_tpid = 0x8100;

    /* set the new default action */
    rv = bcm_vlan_translate_action_id_set( unit, 
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           default_action_id_2,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set \n");
        return rv;
    }

    return rv;
}

/* create new default out-LIF and set its translation edit profile */
int
default_vlan_port_set(int unit, int port) {
    bcm_gport_t default_vlan_port;
    bcm_vlan_t new_vid = 30;
    int action_id_1 = 5;
    int rv;
        
    /* create a new default vlan port (out-Lif) */
    if (!is_device_or_above(unit, JERICHO2)){
        rv = vlan_port_create(unit, port, &default_vlan_port, new_vid, BCM_VLAN_PORT_MATCH_PORT);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_create\n");
            return rv;
        }
    } else {
        bcm_vlan_port_t vp;
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_NONE;
        vp.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_DEFAULT;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n"); 
            print rv;
            return rv;
        }
        default_vlan_port = vp.vlan_port_id;

        bcm_port_match_info_t match_info;
        bcm_port_match_info_t_init(&match_info);
        match_info.match = BCM_PORT_MATCH_PORT;
        match_info.port = port;
        match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;

        /* Add a match to the default vlan port */
        rv = bcm_port_match_add(unit, default_vlan_port, &match_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_match_add $rv\n"); 
            return rv;
        }
    }

    /* set edit profile for egress LIF
       for JR2, there's no new_inner_vlan for virtual egress default profile */
    if (!is_device_or_above(unit, JERICHO2)){
        rv = vlan_port_translation_set(unit, new_vid, new_vid, default_vlan_port, 2, 0);
    } else {
        rv = vlan_port_translation_set(unit, new_vid, 0, default_vlan_port, 2, 0);
    }
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    /* set editing actions*/
    rv = vlan_translate_action_set(unit, action_id_1, 0, 0, bcmVlanActionAdd, bcmVlanActionNone);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_default_translate_action_set\n");
        return rv;
    }

    /* set action class */
    rv = vlan_default_translate_action_class_set(unit, action_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translate_action_class_set\n");
        return rv;
    }

    /* set static entry in MAC table for new vid*/
    rv = l2_addr_add(unit, port, new_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    return 0;
}

/* create vlan_port and set vlan port translation */
int
create_untagged_vlan_port(
    int unit,
    bcm_port_t port_id,
    bcm_gport_t *gport,
    bcm_vlan_t vlan /* incoming outer vlan and also outgoing vlan */, 
    bcm_vlan_t new_vid,
    uint32 edit_class_id,
   uint8 is_ingress
    ) {

    int rv=BCM_E_NONE;

    /* JR2 does not support initial VID,using Port default instead */
    if (!is_device_or_above(unit, JERICHO2)){
        rv = vlan_port_create(unit, port_id, gport, vlan, BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_create\n");
            return rv;
        }
    } else {
        bcm_vlan_port_t vp;
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
        vp.port = port_id;
        vp.vsi = vlan;
        vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n"); 
            print rv;
            return rv;
        }
        *gport = vp.vlan_port_id;
    }

    /* set edit profile for ingress LIF */
    rv = vlan_port_translation_set(unit, new_vid, new_vid, *gport, edit_class_id, is_ingress);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    return rv;
}

/* Set ports TPID class to handle priority and non priority packets as desired
   port1: priority - pass, non priority - pass
   port2: priority - drop, non priority - pass
   port3: priority - delete vlan, non priority (untagged) - add vid */
int
ive_priority_tags_main(int unit, int port1, int port2, int port3, int port4) {
    bcm_gport_t vlan_port;
    int action_id_1=5, action_id_2=6;;
    bcm_port_tpid_class_t port_tpid_class;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t action;
    bcm_vlan_t default_vsi = 1;
    bcm_vlan_t new_vid = 40;
    int rv;
    bcm_port_tag_format_class_t tag_format_ctag = 9;
    bcm_port_tag_format_class_t tag_format_dtag = 3;

    /* set class for all ports */
    rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port1);
        return rv;
    }
    
    rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port2);
        return rv;
    }
    
    rv = bcm_port_class_set(unit, port3, bcmPortClassId, port3);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port3);
        return rv;
    }
    
    rv = bcm_port_class_set(unit, port4, bcmPortClassId, port4);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port4);
        return rv;
    }

    /* JR2: use default application reference to configure port TPID setting */
    if (!is_device_or_above(unit, JERICHO2)){
        /* set TPIDs for port1 */
        port_tpid_init(port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }
        
        /* set TPIDs for port2. Drop priority packets */
        port_tpid_init(port2, 1, 1);
        port_tpid_info1.tpid_class_flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO | BCM_PORT_TPID_CLASS_DISCARD;
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_2\n");
            print rv;
            return rv;
        }
        
        /* set TPIDs for port3 */
        port_tpid_init(port3, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_3\n");
            print rv;
            return rv;
        }
    }

    if (is_device_or_above(unit, JERICHO2)){
        
        /*
         * Create Tag-Struct for C-TAG
         */
        rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeCTag, &tag_format_ctag);
        
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_create, tag_format_class_id=%d, tag_struct_type=bcmTagStructTypeCTag\n", tag_format_ctag);
            return rv;
        }

        /*
         * Create Tag-Struct for S_C-TAG
         */
        rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeSCTag, &tag_format_dtag);

        
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_create, tag_format_class_id=%d, tag_struct_type=bcmTagStructTypeSCTag\n", tag_format_dtag);
            return rv;
        }

        /* set port2 to drop priority tag packet */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port2;
        port_tpid_class.tpid1 = 0x8100;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = tag_format_ctag;
        port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO | BCM_PORT_TPID_CLASS_DISCARD;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port2);
            return rv;
        }
    }

    /* set tag format 2 for priority packets */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port3;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = tag_format_ctag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /* set tag format 0 for untagged packets */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port3;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 0;  
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /*
    * In JR2: No need to connect port to TPIDs,port can point to any supported global TPIDs.
    * Only TPID class is required and done in application reference
    */
    if (!is_device_or_above(unit, JERICHO2)){
        port_tpid_init(port4, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_4\n");
            print rv;
            return rv;
        }
    }

    /* create a vlan port:
       egress_vlan (out vlan) = match_vlan (in vlan) */
	create_untagged_vlan_port(unit, port3, vlan_port, default_vsi, new_vid, 2, 1);

    /* Create action ID 1 */
    rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create 1\n");
        return rv;
    }

    /* Create action ID 2 */
    rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create 2\n");
        return rv;
    }

    /* Set translation action 1. replace outer vlan and add inner. */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionReplace;
    action.dt_inner = bcmVlanActionAdd;
    action.outer_tpid = 0x9100;
    action.inner_tpid = 0x8100;
    rv = bcm_vlan_translate_action_id_set( unit, 
                                           BCM_VLAN_ACTION_SET_INGRESS,
                                           action_id_1,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set 1\n");
        return rv;
    } 

    /* Set translation action 2. add vlan. */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionAdd;
    action.dt_inner = bcmVlanActionNone;
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x9100;
    rv = bcm_vlan_translate_action_id_set( unit, 
                                           BCM_VLAN_ACTION_SET_INGRESS,
                                           action_id_2,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set 1\n");
        return rv;
    } 

    /* Set translation action class - priority packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = 2;
    action_class.tag_format_class_id = tag_format_ctag;
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /* Set translation action class - untagged packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = 2;
    action_class.tag_format_class_id = 0;
    action_class.vlan_translation_action_id = action_id_2;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /* set static entry in MAC table */
    rv = l2_addr_add(unit, port4, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    /* set static entry in MAC table */
    rv = l2_addr_add(unit, port4, new_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    return rv;
}

/* Set port TPID class to handle priority and non priority packets as desired.
   Egress vlan translation: priority - delete vlan, non priority (untagged) - add vid */
int
eve_priority_tags_main(int unit, int port1, int port2) {
    bcm_gport_t vlan_port, in_vlan_port;
    int action_id_1=5, action_id_2=6;;
    bcm_port_tpid_class_t port_tpid_class;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t action;
    bcm_vlan_t default_vsi = 1;
    bcm_vlan_t new_vid = 50;
    int rv;
    bcm_port_tag_format_class_t c_prio_tag_format_class_id = 9;
    uint32 vlan_edit_profile = 2;

    if (is_device_or_above(unit, JERICHO2)){
        rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeCTag ,&c_prio_tag_format_class_id);

        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_create, flags = %d, tag_struct_type = %d, tag_format_class_id=%d, \n",
                        BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeCTag, c_prio_tag_format_class_id);
            return rv;
        }
    }

    /* set class for both ports */
    rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port1);
        return rv;
    }
    
    rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set, port=%d, \n", port2);
        return rv;
    }

    /*
    * set TPIDs for port1
    * In JR2: No need to connect port to TPIDs,port can point to any supported global TPIDs.
    * Only TPID class is required and done in application reference
    */
    if (!is_device_or_above(unit, JERICHO2)){
        port_tpid_init(port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }
        
        /* set TPIDs for port2. */
        port_tpid_init(port2, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_2\n");
            print rv;
            return rv;
        }
    }

    /* classify all incoming packets to default VSI */
    bcm_vlan_port_t vp;
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vp.port = port1;
    vp.vsi = default_vsi;
    vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    /* set tag format 2 for priority packets */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port1;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = c_prio_tag_format_class_id;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port1);
        return rv;
    }

    /* set tag format 2 for priority packets on port2*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port2;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = c_prio_tag_format_class_id;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port2);
        return rv;
    }

    /* set tag format 0 for untagged packets */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port1;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 0;  
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }        

    /* create a vlan port:
       egress_vlan (out vlan) = match_vlan (in vlan) 
       On JR2: VLAN translation entry is done by ESEM with no EEDB.Separate Ingress & Egress configuration
       */
    if (!is_device_or_above(unit, JERICHO2)){
        rv = vlan_port_create(unit, port2, &vlan_port, default_vsi, BCM_VLAN_PORT_MATCH_PORT_VLAN);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_create\n");
            return rv;
        }
    } else {
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = port2;
        vp.vsi = default_vsi;
        vp.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n"); 
            print rv;
            return rv;
        }
        vlan_port = vp.vlan_port_id;
    }

    /* set edit profile for egress LIF */
    rv = vlan_port_translation_set(unit, new_vid, new_vid, vlan_port, vlan_edit_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    /* Create action ID 1 */
    rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create 1\n");
        return rv;
    }

    /* Create action ID 2 */
    rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create 2\n");
        return rv;
    }

    /* Set translation action 1. delete vlan. */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionDelete;
    action.dt_inner = bcmVlanActionNone;
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x9100;
    rv = bcm_vlan_translate_action_id_set( unit, 
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           action_id_1,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set 1\n");
        return rv;
    } 

    /* Set translation action 2. add vlan. */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionAdd;
    action.dt_inner = bcmVlanActionNone;
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x9100;
    rv = bcm_vlan_translate_action_id_set( unit, 
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           action_id_2,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set 1\n");
        return rv;
    } 

    /* Set translation action class - priority packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = vlan_edit_profile;
    action_class.tag_format_class_id = c_prio_tag_format_class_id;
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /* Set translation action class - untagged packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = vlan_edit_profile;
    action_class.tag_format_class_id = 0;
    action_class.vlan_translation_action_id	= action_id_2;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /* set static entry in MAC table */
    rv = l2_addr_add(unit, port2, default_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    /* set static entry in MAC table */
    rv = l2_addr_add(unit, port2, new_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    return rv;
}


/* create vlan_port (Logical interface identified by port-vlan) */
int
vlan_port_create(
    int unit,
    bcm_port_t port_id,  
    bcm_gport_t *gport,
    bcm_vlan_t vlan /* incoming outer vlan and also outgoing vlan */, 
    bcm_vlan_port_match_t criteria
    ) {

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);
  
    vp.criteria = criteria;
    vp.port = port_id;
    vp.match_vlan = vlan; 
    vp.vsi = vlan;
    vp.flags = 0;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;  
    return BCM_E_NONE;
}

/* set translation actions (replace) */
int
vlan_default_translate_action_set(int unit, int action_id_1) {
    bcm_vlan_action_set_t action;
    uint32 flags;
    int rv;

    flags = BCM_VLAN_ACTION_SET_EGRESS;

    /* Create action IDs*/
    rv = bcm_vlan_translate_action_id_create( unit, flags | BCM_VLAN_ACTION_SET_WITH_ID, &action_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /* Set translation action 1. for incoming untagged packets. */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionAdd;
    action.dt_inner = bcmVlanActionNone;
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x9100;
    rv = bcm_vlan_translate_action_id_set( unit, 
                                           flags,
                                           action_id_1,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set 1\n");
        return rv;
    }  
    
    return rv; 
}

/* set translation action classes for different tag formats */
int
vlan_translate_action_class_set(int unit, int action_id_1, int action_id_2, uint8 is_ingress ) {
    bcm_vlan_translate_action_class_t action_class;
    int rv;

    /* Set translation action class for single tagged packets with TPID 0x8100 */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = 2;
    /* JR2 use tag format id 8 for ctag TPID 0x8100 */
    if (!is_device_or_above(unit, JERICHO2)){
        action_class.tag_format_class_id = 2;
    } else {
        action_class.tag_format_class_id = 8;
    }
    action_class.vlan_translation_action_id	= action_id_1;
    action_class.flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /* Set translation action class for single tagged packets with TPID 0x9100 */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = 2;
    /* JR2 use tag format id 4 for stag TPID 0x9100 */
    if (!is_device_or_above(unit, JERICHO2)){
        action_class.tag_format_class_id = 3;
    } else {
        action_class.tag_format_class_id = 4;
    }
    action_class.vlan_translation_action_id	= action_id_2;
    action_class.flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;
}

/* add static entry to MAC table */
int
l2_addr_add(
    int unit, 
    bcm_gport_t port, 
    uint16 vid
    ) {

    bcm_l2_addr_t l2addr;
    bcm_mac_t mac  = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    int rv;

    bcm_l2_addr_t_init(&l2addr, mac, vid);

    l2addr.port = port;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

/* Note: QOS mapping in this context is performed as part of vlan translation, mainly pcp manilpulation.
 *  We have three functions dedicated for this:
 *  set_qos_default_settings(): sets the port default priority for an untagged packet.
 *  add_qos_mapping(): Define a mapping and a mapping type; Define the mapping inputs, either inner or outer tag. 
 *                     For instance: we map an incoming outer-tag pcp to a new outer-tag pcp.
 *  set_qos_mapping(): Attach the mapping to a {port X vlan} object ,as part of the vlan translation setting.
 *  
 *  After performing a mapping, we need to set the (vlan translation)action->priority field to be the QOS profile mapping id in case of action Add/Replace.
 */


/* First, we set the port default priority for an untagged packet.
 * Second, we set a global drop precedence default assignment. 
 * parameters: 1. port: The port upon which we set the default priority. 
 *             2. priority: This is the port default priority.
 *             3. color: This value is used as a global drop precedence. 
 */
int set_qos_default_settings(int unit, bcm_port_t port, int priority, int color){

    int rv = BCM_E_NONE;

    rv = bcm_port_untagged_priority_set(unit, port, priority);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_untagged_priority_set\n");
        return rv;
    }

    rv = bcm_port_control_set(unit, -1, bcmPortControlDropPrecedence, color);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    return rv;
}


/*
 * This function performs two steps: 
 * 1. (optional) Allocate a QOS map id.  
 * 2. Add mapping entries. 
 * parameters: 1. creation_flags: In our context we want to pass:  flags == (BCM_QOS_MAP_INGRESS or BCM_QOS_MAP_EGRESS)| BCM_QOS_MAP_L2_VLAN_PCP; 
 *             2. addition_flags: pass $::BCM_QOS_MAP_L2_OUTER_TAG for outer tag, BCM_QOS_MAP_L2_INNER_TAG for inner tag,
 *                BCM_QOS_MAP_L2_UNTAGGED for untagged.
 *             3. map_id: the id given in  bcm_qos_map_create. If we only want to add a mapping with no create, pass a value after performing create.
 *             4. map_struct: contains information relevant for the mapping. This will hold the actual mapping between the old pcp to the new one.
 *             5. color: a value that is mapped to a certain priority profile.
 *             6. internal_priority: pass the old pcp value (inner or outer tag pcp).
 *             7. packet_priority: pass the new pcp value.
 *             8. with_create == 1: create a mapping id; with_create==0: Don;t create a mapping id. bcm_qos_map_add will be performed
 *                assuming a valid mapping id has been passed.
 */
/* int add_qos_mapping(int unit , uint32 creation_flags, uint32 addition_flags, int *map_id, bcm_qos_map_t *map_struct, bcm_color_t color,  uint8 internal_priority, int packet_priority, int with_create){*/
int add_qos_mapping(int unit , uint32 creation_flags, uint32 addition_flags, bcm_color_t color,  uint8 internal_priority, int packet_priority, int qos_map_id, int with_create){

    int rv = BCM_E_NONE;
    bcm_qos_map_t map_struct;

    bcm_qos_map_t_init(&map_struct);

    if (with_create) {
        rv = bcm_qos_map_create(unit, creation_flags, &qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_qos_map_create\n");
            return rv;
        }
    }

    if (creation_flags & BCM_QOS_MAP_INGRESS) {
        ingress_qos_map_id = qos_map_id;
    }
    else{
        egress_qos_map_id = qos_map_id;
    }


    map_struct.int_pri = internal_priority;
    map_struct.pkt_pri = packet_priority;
    map_struct.pkt_cfi = 0;
    if (color > 0) {
        map_struct.color = color;
    }
    
    addition_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP);
    
    rv = bcm_qos_map_add(unit, addition_flags, map_struct, qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_qos_map_create\n");
        return rv;
    }

    return rv;
}


/*
 * After creating  mapping entries, we want to attach them to a vlan_port. 
 * parameters: 1. port: vlan_port id of the vlan_port to which this mapping is attached.
 *             2. ingress_map: ingress map id.
 *             3. egress_map: egress map id.
 */
int set_qos_mapping(int unit, bcm_gport_t port, int ingress_map, int egress_map){

    int rv = BCM_E_NONE;

    rv = bcm_qos_port_map_set(unit, port, ingress_map, egress_map);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_qos_port_map_set\n");
        return rv;
    }

    return rv;
}

int get_ingress_qos_map_id(){
    return ingress_qos_map_id;
}

int get_egress_qos_map_id(){
    return egress_qos_map_id;
}

/* Sets qos mapping in AVT mode */
int set_qos_mode() {
	int rv = BCM_E_NONE;

    qos_mode = 1;

	return rv;
}
