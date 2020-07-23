/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Basic VPLS example with only sap support
 *
 * It provides a general example of how to configure vpls service on DNX devices.
 *
 * How to run:
 *      cd ../../../../src/
 *      cint examples/dpp/cint_vpls_saps.c
 *      cint
 *      vpls_run(0,1);
 *      create_sap(0,14,100);
 *      exit;
 *      tx 1 PSRC=14 DATA=0x000000000002000000000005810000640899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c
 *
 * - The packet must be send from in_port in the cint and has the same vlan id as the configured in cint.
 */

typedef struct tSapEntryInfo
{
    int configured;
    int port;
    int vlan;
    int vlan_port_id;
    int encap_id;
};

typedef struct cint_vpls_saps_info_s {
    int DEFAULT_VLAN_ETYPE;
    int DEFAULT_INDEX;
    int DUNE_NULL_ENCAP_ID;
    int DUNE_RCYCLE_IF_MC_OFFSET;
    int default_vpls_recycle_port_num;
    int use_ingress_mc_group;
    bcm_vpn_t vpn;
    bcm_multicast_t egress_mc_group;
    bcm_multicast_t ingress_mc_group;
    int gMaxNumberOfSaps;
    int gCurrentNumberOfSapsCreated;
        
    /* Values for the old vlan translation actions and profiles */
    int INGRESS_REMOVE_TAGS_PROFILE;
    int INGRESS_REMOVE_OUTER_TAG_ACTION;
    int EGRESS_NOP_PROFILE;
    int EGRESS_REMOVE_TAGS_PROFILE;
    int EGRESS_REMOVE_TAGS_PUSH_1_PROFILE;
    int EGRESS_REMOVE_TAGS_PUSH_2_PROFILE;
    int EGRESS_UNTAGGED_OFFSET;
    int EGRESS_SINGLE_TAGGED_OFFSET;
    int EGRESS_DOUBLE_TAGGED_OFFSET;
    
    /*                          [gMaxNumberOfSaps] */
    tSapEntryInfo sapConfigArray[10];
};

cint_vpls_saps_info_s cint_vs_info = {
    /* DEFAULT_VLAN_ETYPE | DEFAULT_INDEX | DUNE_NULL_ENCAP_ID | DUNE_RCYCLE_IF_MC_OFFSET */
    0x8100,                 -1,             0xFFFFFFFF,          40000,    
    /* default_vpls_recycle_port_num | use_ingress_mc_group | vpn | egress_mc_group | ingress_mc_group */
    101,                               1,                     0,    0,                0,
    /* gMaxNumberOfSaps | gCurrentNumberOfSapsCreated */
    10,                   0,
    /* INGRESS_REMOVE_TAGS_PROFILE | INGRESS_REMOVE_OUTER_TAG_ACTION | EGRESS_NOP_PROFILE | EGRESS_REMOVE_TAGS_PROFILE */
    4,                               5,                                0,                   2,
    /* EGRESS_REMOVE_TAGS_PUSH_1_PROFILE | EGRESS_REMOVE_TAGS_PUSH_2_PROFILE | EGRESS_UNTAGGED_OFFSET | EGRESS_SINGLE_TAGGED_OFFSET */
    5,                                     8,                                  0,                       1,
    /* EGRESS_DOUBLE_TAGGED_OFFSET */
    2
};

int allocate_sap_entry(int unit, int port, int vlan, tSapEntryInfo **sapPtr)
{
    int            sapEntryIndex;
    tSapEntryInfo *sapEntryPtr;

    /* check if the sap already exists or not */
    for (sapEntryIndex = 0; sapEntryIndex < cint_vs_info.gMaxNumberOfSaps; sapEntryIndex++)
    {
        sapEntryPtr = &(cint_vs_info.sapConfigArray[sapEntryIndex]);

        if ((sapEntryPtr->configured) &&
            (sapEntryPtr->port == port) &&
            (sapEntryPtr->vlan == vlan))
        {
            return BCM_E_EXISTS;
        }
    }

    /* find a free entry to add the sap info into */
    for (sapEntryIndex = 0; sapEntryIndex < cint_vs_info.gMaxNumberOfSaps; sapEntryIndex++)
    {
        sapEntryPtr = &(cint_vs_info.sapConfigArray[sapEntryIndex]);

        if (!sapEntryPtr->configured)
        {
            sapEntryPtr->configured = TRUE;
            sapEntryPtr->port       = port;
            sapEntryPtr->vlan       = vlan;
            *sapPtr                 = sapEntryPtr;

            cint_vs_info.gCurrentNumberOfSapsCreated++;

            return BCM_E_NONE;
        }
    }

    return BCM_E_FULL;
}

int set_port_outer_tpid(int unit, int port, int tpid, int vlan_transation_profile_id)
{
    int                   rv;
    bcm_port_tpid_class_t port_tpid_class;

    /* remove old tpids on port */
    rv = bcm_port_tpid_delete_all(unit, port);
    if (rv != BCM_E_NONE) {
        printf("Error (%s), in bcm_port_tpid_delete_all, unit=%d, port=%d\n", bcm_errmsg(rv), unit, port);
        return rv;
    }

    /* set outer tpids of the port */
    rv = bcm_port_tpid_add(unit, port, tpid, 0 /*color_set*/);
    if (rv != BCM_E_NONE) {
        printf("Error (%s), in bcm_port_tpid_add, tpid=%d for port %u\n", bcm_errmsg(rv), tpid, port);
        return rv;
    }

    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = tpid;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = 2;  /* can be any number !=0, 2 is backward for s-tag */
    port_tpid_class.flags = 0;
    port_tpid_class.vlan_translation_action_id = vlan_transation_profile_id;
    
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d\n", port);
        return rv;
    }

    return BCM_E_NONE;
}

int create_sap(int unit, int port, int vlan)
{
    int                          rv;
    bcm_vlan_port_t              vlan_port;
    bcm_mac_t                    mac = {0x00,0x00,0x00,0x00,0x00,vlan};
    bcm_l2_addr_t                l2_addr;
    bcm_vlan_port_translation_t  port_trans;
    tSapEntryInfo               *sapEntryPtr;

    rv = allocate_sap_entry(unit, port, vlan, &sapEntryPtr);
    if (rv != BCM_E_NONE)
    {
        printf("allocate_sap_entry failure! Current number of SAPs created: %u. Max is %u\n",
                cint_vs_info.gCurrentNumberOfSapsCreated, cint_vs_info.gMaxNumberOfSaps);
        return rv;
    }

    /*                                                                   vlan_transation_profile_id */
    rv = set_port_outer_tpid(unit, port, cint_vs_info.DEFAULT_VLAN_ETYPE, 0);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

	rv = bcm_switch_control_port_set(0, port, bcmSwitchTrunkHashPktHeaderSelect, BCM_HASH_HEADER_FORWARD);
    if (rv != BCM_E_NONE)
    {
        printf ("bcm_switch_control_port_set - bcmSwitchTrunkHashPktHeaderSelect failed (%s) for port %u, headerType %u\n",
                 bcm_errmsg(rv), port, 1);
    }

    /* select number of headers to consider in LAG hashing */
    rv = bcm_switch_control_port_set(0, port, bcmSwitchTrunkHashPktHeaderCount, 3);
    if (rv != BCM_E_NONE)
    {
        printf ("bcm_switch_control_port_set - bcmSwitchTrunkHashPktHeaderCount failed (%s) for port %u, hdrCount %u\n",
                 bcm_errmsg(rv), port, 3);
    }

    /* add port, according to port_vlan_vlan */
    bcm_vlan_port_t_init(&vlan_port);

    /* set port attribures, key <port-vlan> */
    vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port        = port;
    vlan_port.match_vlan  = vlan;
    vlan_port.egress_vlan = vlan;
    vlan_port.flags       = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error (%s), bcm_vlan_port_create for port %u, vlan %u\n", bcm_errmsg(rv), port, vlan);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, cint_vs_info.vpn, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error (%s), bcm_vswitch_port_add failed for vplsIndex %u and gport 0x%x\n",
                bcm_errmsg(rv),
                cint_vs_info.vpn,
                vlan_port.vlan_port_id);
        return rv;
    }

    /* Set vlan translation */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = vlan;
    port_trans.new_inner_vlan = 0;
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = cint_vs_info.INGRESS_REMOVE_TAGS_PROFILE;
    port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set for ingress!\n");
        return rv;
    }

    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = vlan;
    port_trans.new_inner_vlan = 0;
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = cint_vs_info.EGRESS_REMOVE_TAGS_PUSH_1_PROFILE;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set for egress!\n");
        return rv;
    }

	if (cint_vs_info.use_ingress_mc_group)
	{
	    rv = bcm_multicast_ingress_add(unit, cint_vs_info.ingress_mc_group, port, vlan_port.encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error (%s), bcm_multicast_ingress_add: port %d encap_id 0x%x to ingress_mc_group 0x%x \n",
                 bcm_errmsg(rv), port, vlan_port.encap_id, cint_vs_info.ingress_mc_group);
            return rv;
        }
	} else {
	    rv = bcm_multicast_egress_add(unit, cint_vs_info.egress_mc_group, port, vlan_port.encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error (%s), bcm_multicast_egress_add: port %d encap_id  0x%x to egress_mc_group 0x%x \n",
                 bcm_errmsg(rv), port, vlan_port.encap_id, cint_vs_info.egress_mc_group);
            return rv;
        }
	}
	
    printf("SUCCESS at sap addition for port %u and vlan %u (gport 0x%x, encap 0x%x)!\n ",
             port,vlan,vlan_port.vlan_port_id, vlan_port.encap_id);

    sapEntryPtr->port         = port;
    sapEntryPtr->vlan         = vlan;
    sapEntryPtr->vlan_port_id = vlan_port.vlan_port_id;
    sapEntryPtr->encap_id     = vlan_port.encap_id;

    bcm_l2_addr_t_init(&l2_addr, mac, cint_vs_info.vpn);
    l2_addr.flags = BCM_L2_STATIC; /* static entry */
    l2_addr.port = vlan_port.vlan_port_id;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error (%s), in bcm_l2_addr_add for vpls %u\n",  bcm_errmsg(rv), cint_vs_info.vpn);
        return rv;
    }
	
    printf("SUCCESS at L2 fdb addition (%02x:%02x:%02x:%02x:%02x:%02x) for gport 0x%x and encap 0x%x!\n ",
             mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],
			 vlan_port.vlan_port_id, vlan_port.encap_id);

    return rv;
}

int create_vpls_service(int unit, bcm_vpn_t index)
{
    int bcm_error = BCM_E_NONE;
    bcm_mpls_vpn_config_t info = {0};
    unsigned int flags = BCM_MULTICAST_TYPE_VPLS | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP;

    cint_vs_info.vpn = index + 0x7000;

    cint_vs_info.egress_mc_group = cint_vs_info.vpn;
 
    /* destroy before open, to ensure it not exist */
    bcm_multicast_destroy(unit, cint_vs_info.egress_mc_group);

    /* Allocate a multicast group index of type VPLS. */
    if ((bcm_error = bcm_multicast_create (unit, flags, &(cint_vs_info.egress_mc_group))) != BCM_E_NONE) {
        printf ("bcm_multicast_create failed, group %d  rv=%s\n", cint_vs_info.egress_mc_group, bcm_errmsg(bcm_error));
        cint_vs_info.egress_mc_group = cint_vs_info.DEFAULT_INDEX;
        return BCM_E_INTERNAL;
    }

    if (cint_vs_info.use_ingress_mc_group)
    {
		bcm_gport_t  gport_recycle_tls_multicast = 0;

		cint_vs_info.ingress_mc_group = cint_vs_info.vpn + cint_vs_info.DUNE_RCYCLE_IF_MC_OFFSET;
		flags = BCM_MULTICAST_TYPE_VPLS | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP;
		
        /* destroy before open, to ensure it not exist */
        bcm_multicast_destroy(unit, cint_vs_info.ingress_mc_group);

        if ((bcm_error = bcm_multicast_create (unit, flags, &(cint_vs_info.ingress_mc_group))) != BCM_E_NONE) {
            printf ("bcm_multicast_create failed for ingress_mc_group %d  rv=%s\n", cint_vs_info.ingress_mc_group, bcm_errmsg(bcm_error));
            cint_vs_info.ingress_mc_group = cint_vs_info.DEFAULT_INDEX;
            return BCM_E_INTERNAL;
        }
		
		BCM_GPORT_LOCAL_SET(gport_recycle_tls_multicast, cint_vs_info.default_vpls_recycle_port_num);
		
		bcm_error = bcm_multicast_egress_add(unit, cint_vs_info.egress_mc_group, gport_recycle_tls_multicast, cint_vs_info.DUNE_NULL_ENCAP_ID);
        if (bcm_error != BCM_E_NONE)
        {
            printf("Error (%s), bcm_multicast_egress_add: recycle_gport 0x%x to egress_mc_group 0x%x in unit %u\n",
                   bcm_errmsg(bcm_error),
                   gport_recycle_tls_multicast,
                   cint_vs_info.egress_mc_group,
                   unit);
            return BCM_E_INTERNAL;
		}
    } 


    info.unknown_multicast_group = cint_vs_info.egress_mc_group;
    info.unknown_unicast_group   = cint_vs_info.egress_mc_group;
    info.broadcast_group         = cint_vs_info.egress_mc_group;
    info.flags                   = BCM_MPLS_VPN_VPLS | BCM_MPLS_VPN_WITH_ID;
    info.vpn                     = cint_vs_info.vpn;

    /* Create a L2 VPN for this service. */
    if ((bcm_error = bcm_mpls_vpn_id_create (unit, &info) != BCM_E_NONE))
    {
        printf ("bcm_mpls_vpn_id_create failed, vpn 0x%x rv=%s", info.vpn, bcm_errmsg(bcm_error));
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Fill Action
 */
int
vlan_translation_action_add(int unit, int action_id, int flags, bcm_vlan_action_t outer_vid_source, bcm_vlan_action_t inner_vid_source, int outer_tpid, int inner_tpid)
{
    int rv;
    bcm_vlan_action_set_t action;

    /* Create action ID*/
    bcm_vlan_action_set_t_init(&action);

    action.dt_outer = outer_vid_source;
    action.dt_inner = inner_vid_source;
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;

    rv = bcm_vlan_translate_action_id_set( unit,
                                           flags,
                                           action_id,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    return rv;
}
/*
 * Creates an action with the given id and configures it.
 * If given an edit profile, map it to this action + OFFSET if it's an egress action.
 */
int
vlan_translation_create_mapping(int unit, int action_id, bcm_vlan_action_t outer_vid_source, bcm_vlan_action_t inner_vid_source,
                                int edit_profile, uint8 is_ingress){

    int flags;
    int rv;
    char *ineg = (is_ingress) ? "ingress" : "egress";
    bcm_vlan_translate_action_class_t action_class;
    int tag_format;
    uint16 outer_tpid = 0x8100;
    uint16 inner_tpid = 0x8100;

    /* Create new Action IDs */
    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    flags |= BCM_VLAN_ACTION_SET_WITH_ID;
    rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    printf("Created %s action id %d\n", ineg, action_id);

    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    rv = vlan_translation_action_add(unit, action_id, flags, outer_vid_source, inner_vid_source, outer_tpid, inner_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translation_action_add\n");
        return rv;
    }

    printf("Created %s translation action %d\n", ineg, action_id);

    /* In case we don't want to create mapping, return here. */
    if (edit_profile < 0) {
        return rv;
    }

    /* Map edit profile and all tag formats to command */
    for (tag_format = 0 ; tag_format < 16 ; tag_format++) {
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = edit_profile;
        action_class.tag_format_class_id = tag_format;

        /*
         * Tag format 0           ==> will be mapped to ingress action NOP
         * Tag format 2 (one-tag) ==> will be mapped to REMOVE_OUTER_TAG
         * else                   ==> will be mapped to action_id
         */
        if (is_ingress) {
                 action_class.vlan_translation_action_id = (tag_format == 0) ? 0
                                                            : (tag_format == 2) ? cint_vs_info.INGRESS_REMOVE_OUTER_TAG_ACTION
                                                            :  action_id;

        } else {
                action_class.vlan_translation_action_id = (tag_format == 2) ? action_id + cint_vs_info.EGRESS_SINGLE_TAGGED_OFFSET
                                                            : (tag_format == 6) ? action_id + cint_vs_info.EGRESS_DOUBLE_TAGGED_OFFSET
                                                            :  action_id;
        }

        action_class.flags = flags;
        rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }

    printf("Mapped %s edit profile %d and all tag_format to %s action %d\n", ineg, edit_profile, ineg, action_id);

    return rv;
}


/*
    REMOVE_TAGS: To remove vlan tags from the packet

    Then, For each profile, we map the relevant tag to its action.
*/
int
create_default_ingress_actions(int unit)
{
    int rv;
    int in_action_id;

    /* CREATE INGRESS ACTION 4: remove tags, mapped from edit profile 1 and all tag formats */
    in_action_id = cint_vs_info.INGRESS_REMOVE_TAGS_PROFILE;
    rv = vlan_translation_create_mapping(unit, in_action_id, bcmVlanActionDelete, bcmVlanActionDelete, in_action_id, 1);
    if (rv != BCM_E_NONE) {
        printf("Error: vlan_translation_create_mapping INGRESS_REMOVE_TAGS_PROFILE\n");
        return rv;
    }

    return rv;
}

/*
   9 Egress actions are created:
   For each type of packet tag format (untagged, single tagged or double tagged) we create three profiles:
        REMOVE_TAGS: to remove all vlan tags
        REMOVE_TAGS_PUSH_1: to remove all tags, and push a new one
        REMOVE_TAGS_PUSH_2: to remove all tags, and push two new ones

   Then, For each profile, we map the relevant tag to its action.
*/
int
create_default_egress_actions(int unit){
    int rv;
    int eg_action_id;
    int eg_profile;

    /* Second action type: remove tags, push 1 */
    eg_profile = cint_vs_info.EGRESS_REMOVE_TAGS_PUSH_1_PROFILE;

    /* Egress action 6: get single tagged, remove tags push 1 */
    eg_action_id = cint_vs_info.EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + cint_vs_info.EGRESS_SINGLE_TAGGED_OFFSET;
    rv = vlan_translation_create_mapping(unit, eg_action_id , bcmVlanActionReplace, bcmVlanActionNone, -1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error: vlan_translation_create_mapping EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + EGRESS_SINGLE_TAGGED_OFFSET\n");
        return rv;
    }

    /* Egress action 7: get double tagged, remove tags push 1 */
    eg_action_id = cint_vs_info.EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + cint_vs_info.EGRESS_DOUBLE_TAGGED_OFFSET;
    rv = vlan_translation_create_mapping(unit, eg_action_id , bcmVlanActionReplace, bcmVlanActionDelete, -1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error: vlan_translation_create_mapping EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + EGRESS_DOUBLE_TAGGED_OFFSET\n");
        return rv;
    }

    /* Egress action 5: get untagged, remove tags push 1
       Also, map all remove tags push 1 actions */
    eg_action_id = cint_vs_info.EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + cint_vs_info.EGRESS_UNTAGGED_OFFSET;
    rv = vlan_translation_create_mapping(unit, eg_action_id , bcmVlanActionAdd, bcmVlanActionNone, eg_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error: vlan_translation_create_mapping EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + EGRESS_UNTAGGED_OFFSET\n");
        return rv;
    }

    return rv;
}

int
vlanTranslationDefaultModeInit(int unit)
{
    int rv;

    /* CREATE INGRESS ACTIONS */
    rv = create_default_ingress_actions(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_default_ingress_actions\n");
        return rv;
    }

    /* CREATE  EGRESS ACTIONS */

    rv = create_default_egress_actions(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_default_eggress_actions\n");
        return rv;
    }

    printf("Done configuring default vlan translation settings for unit %d\n", unit);

    return rv;
}

int
vpls_run(int unit, int vplsIndex)
{
    int bcm_error = BCM_E_NONE;
    
    /* Set VLAN translation advanced mode */
    bcm_error = bcm_switch_control_set(unit, bcmSwitchPortVlanTranslationAdvanced,1);
    if (bcm_error != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set bcm_error %s\n",  bcm_errmsg(bcm_error));
        return bcm_error;
    }

    /* Set defualt behaviour for Ingress and Egress VLAN translation actions */
    bcm_error = vlanTranslationDefaultModeInit(unit);
    if (bcm_error != BCM_E_NONE) {
        printf("Error, in vlanTranslationDefaultModeInit\n");
        return bcm_error;
    }

    bcm_error = create_vpls_service(unit, vplsIndex);
    if (bcm_error != BCM_E_NONE)
    {
        printf("Error, in create_vpls_service\n");
        return bcm_error;
    }

    printf("Vpls %u has been created successfully with internal vpn 0x%x\n", vplsIndex, cint_vs_info.vpn);
    
    return bcm_error;
}

