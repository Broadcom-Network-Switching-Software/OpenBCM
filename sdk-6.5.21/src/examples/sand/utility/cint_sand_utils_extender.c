/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides Port Extender basic functionality
 */


/****************************************************************************************************
 ********                      Global Variables Definition and Initialization                ********
 */

struct extender__port_extender_port_s {
    bcm_gport_t port;
    bcm_vlan_t tag_vid;                         /* Outer C-VID */
    uint16 extender_vid;                        /* Extended-Port E-CID */
    bcm_gport_t extender_gport_id;              /* The LIF ID */
};

int EXTENDER__PMF_FIELD_GRP_ID_RESTORE_ETAG     = 1;
int EXTENDER__PMF_FIELD_GRP_ID_ING_VALIDATION   = 2;

/* flag for the wide lif */
int is_extender_port_wide_lif = 0;


/****************************************************************************************************
 ********                                   Functions Declaration                            ********
 */
int extender__init(int unit, uint16 etag_ethertype);
int extender__cascaded_port_configure(int unit, int port);
int extender__extender_port_configure(int unit, extender__port_extender_port_s *extender_port_info);
int extender__extender_port_untagged_configure(int unit, extender__port_extender_port_s *extender_port_info);
int extender__extender_port_cleanup(int unit, bcm_gport_t gport);
int extender__cleanup(int unit);


/* utility functions (used internally) */
int extender__pmf_init(int unit);
int extender__pmf_port_rules_add(int unit, int port);
int extender__pmf_cleanup(int unit, int group_id);



/****************************************************************************************************
 ********                                   Functions Implementation                         ********
 */


/* Init function (run only once):
   - Verify per device SOC properties
   - Install PMF field groups
   - Set global Etag Ethertype
*/
int extender__init(int unit, uint16 etag_ethertype)
{
    bcm_error_t rv = BCM_E_NONE;
    int cb_enable, prepend_bytes, prepend_offset;

    if (!is_device_or_above(unit, JERICHO2)) {
        /* Verify SOC properties */
        cb_enable = soc_property_get(unit ,"extender_control_bridge_enable", 0);
        if (!cb_enable) {
            printf("Error in extender__init, cb_enable - %d\n", cb_enable);
            return BCM_E_PARAM;
        }

        prepend_bytes = soc_property_get(unit ,"prepend_tag_bytes", 0);
        if (prepend_bytes != 8) {
            printf("Error in extender__init, prepend_bytes - %d\n", prepend_bytes);
            return BCM_E_PARAM;
        }

        prepend_offset = soc_property_get(unit ,"prepend_tag_offset", 0);
        if (prepend_offset) {
            printf("Error in extender__init, prepend_offset - %d\n", prepend_offset);
            return BCM_E_PARAM;
        }

        /* Configure extender pmf field groups */
        rv = extender__pmf_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, extender__pmf_init failed \n");
            return rv;
        }
    }

    /* Set the global Etag ethertype */
    rv = bcm_switch_control_set(unit, bcmSwitchEtagEthertype, etag_ethertype);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set failed for bcmSwitchEtagEthertype, etag_ethertype: 0x%x\n", etag_ethertype);
        return rv;
    }


    return BCM_E_NONE;
}

/*
 *  Init function - Run for each cascaded port
 */
int extender__cascaded_port_configure(int unit, int port)
{
    int rv;

    /* Enable prepend operation for the port */
    rv = bcm_switch_control_port_set(unit, port, bcmSwitchPrependTagEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_switch_control_set for bcmSwitchPrependTagEnable, port - %d, rv - %d\n", port, rv);
        return rv;
    }

    /* Configure the Port Extender Port-type to be a Cascaded Port */
    rv = bcm_port_control_set(unit, port, bcmPortControlExtenderType, BCM_PORT_EXTENDER_TYPE_SWITCH);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set for bcmPortControlExtenderType to SWITCH, port - %d, rv - %d\n", port, rv);
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        /* Drop the untagged channel */
        rv = bcm_port_control_set(unit, port, bcmPortControlNonEtagDrop, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set for bcmPortControlNonEtagDrop to Enable, port - %d, rv - %d\n", port, rv);
            return rv;
        }
    

        /* Create PMF rules to restore the ETAG of packets and ingress Validation */
        rv = extender__pmf_port_rules_add(unit, port);
        if (rv != BCM_E_NONE) {
            printf("Error, extender__pmf_port_rules_add, port - %d, rv - %d\n", port, rv);
            return rv;
        }
    }

    return rv;
}

/* Configuration of a Port Extender single Extender LIF
 * Create the object(LIF) and register ECID
 *
 * INPUT:
 *   extender_port_info: Configuration info for a single Extender LIF.
 */
int extender__extender_port_configure(int unit, extender__port_extender_port_s *extender_port_info) {

    int rv;
    bcm_extender_port_t extender_port;
    bcm_extender_forward_t fwd_entry;

    /* Format the supplied LIF info to the Port-Extender struct */
    bcm_extender_port_t_init(&extender_port);
    extender_port.port = extender_port_info->port;
    extender_port.match_vlan = extender_port_info->tag_vid;
    extender_port.extended_port_vid = extender_port_info->extender_vid;

    extender_port.pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG;

    /** application here always for match ctag+etag+port */
    if (is_device_or_above(unit, JERICHO2)) {
        extender_port.criteria = BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_CVLAN;
    }

    if (is_extender_port_wide_lif) {
        extender_port.flags |= BCM_EXTENDER_PORT_INGRESS_WIDE;
    }

    /* Call the API to create the Port Extender object */
    rv = bcm_extender_port_add(unit, &extender_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_extender_port_add, port - %d, vlan - %d, E-TAG - %d, rv - %d\n", extender_port.port, extender_port.match_vlan, extender_port.extended_port_vid, rv);
        return rv;
    }

    printf("Extender port params. port: %d, VLAN: %d ECID: %d\n", extender_port_info->port, extender_port_info->tag_vid, extender_port_info->extender_vid);

    extender_port_info->extender_gport_id = extender_port.extender_port_id;

    if (!is_device_or_above(unit, JERICHO2)) {
        /* Add E-Channel registration entry */
        bcm_extender_forward_t_init(&fwd_entry);
        fwd_entry.flags = BCM_EXTENDER_FORWARD_UPSTREAM_ONLY;
        fwd_entry.name_space = extender_port_info->port; /* vlan_doamin */
        fwd_entry.extended_port_vid = extender_port_info->extender_vid; /* ecid */
        rv = bcm_extender_forward_add(unit, &fwd_entry);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_extender_forward_add for port - %d, ecid - 0x%x rv - %d\n", extender_port_info->port, extender_port_info->extender_vid, rv);
            return rv;
        }
    }
    return BCM_E_NONE;
}

/* Configuration of a Port Extender single Extender LIF
 * Create the object(LIF) and register ECID
 *
 * INPUT:
 *   extender_port_info: Configuration info for a single Extender LIF.
 */
int extender__extender_port_untagged_configure(int unit, extender__port_extender_port_s *extender_port_info) {

    int rv;
    bcm_extender_port_t extender_port;
    bcm_extender_forward_t fwd_entry;

    /* Format the supplied LIF info to the Port-Extender struct */
    bcm_extender_port_t_init(&extender_port);
    extender_port.port = extender_port_info->port;
    if (!is_device_or_above(unit, JERICHO2)) {
        extender_port.flags = BCM_EXTENDER_PORT_INITIAL_VLAN;
        extender_port.match_vlan = extender_port_info->tag_vid;
    } else {
        extender_port.criteria = BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_UNTAGGED;
    }
    
    extender_port.extended_port_vid = extender_port_info->extender_vid;

    extender_port.pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG;

    if (is_extender_port_wide_lif) {
        extender_port.flags |= BCM_EXTENDER_PORT_INGRESS_WIDE;
    }

    /* Call the API to create the Port Extender object */
    rv = bcm_extender_port_add(unit, &extender_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_extender_port_add, port - %d, vlan - %d, E-TAG - %d, rv - %d\n", extender_port.port, extender_port.match_vlan, extender_port.extended_port_vid, rv);
        return rv;
    }

    printf("Extender port params. port: %d, VLAN: %d ECID: %d\n", extender_port_info->port, extender_port_info->tag_vid, extender_port_info->extender_vid);

    extender_port_info->extender_gport_id = extender_port.extender_port_id;

    if (!is_device_or_above(unit, JERICHO2)) {
        /* Add E-Channel registration entry */
        bcm_extender_forward_t_init(&fwd_entry);
        fwd_entry.flags = BCM_EXTENDER_FORWARD_UPSTREAM_ONLY;
        fwd_entry.name_space = extender_port_info->port; /* vlan_doamin */
        fwd_entry.extended_port_vid = extender_port_info->extender_vid; /* ecid */
        rv = bcm_extender_forward_add(unit, &fwd_entry);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_extender_forward_add for port - %d, ecid - 0x%x rv - %d\n", extender_port_info->port, extender_port_info->extender_vid, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Configuration of a Port Extender single Extender OutLIF
 * Create the OutLIF
 *
 * INPUT:
 *   extender_port_info: Configuration info for a single Extender OutLIF.
 */
int extender__extender_port_mc_configure(int unit, extender__port_extender_port_s *extender_port_info) {

    int rv;
    bcm_extender_port_t extender_port;

    /* Format the supplied LIF info to the Port-Extender struct */
    bcm_extender_port_t_init(&extender_port);
    extender_port.extended_port_vid = extender_port_info->extender_vid;
    extender_port.pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG;
    extender_port.flags = BCM_EXTENDER_PORT_MULTICAST;
    if (is_device_or_above(unit, JERICHO2)) {
        extender_port.criteria = BCM_EXTENDER_PORT_MATCH_PORT_EXTENDED_PORT_VLAN;
    }

    /* Call the API to create the Port Extender object */
    rv = bcm_extender_port_add(unit, &extender_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_extender_port_add, port - %d, rv - %d\n", extender_port_info->port, rv);
        return rv;
    }

    extender_port_info->extender_gport_id = extender_port.extender_port_id;
    return rv;
}


/* Create PMF rules to:
   - Restore the ETAG of packets
   - Verify E-Channel registration and no Etag filter (untagged)
   */
int extender__pmf_init(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t group_cfg1;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    rv = bcm_field_group_create_mode_id(unit, qset, 10, bcmFieldGroupModeAuto, EXTENDER__PMF_FIELD_GRP_ID_RESTORE_ETAG);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR:: bcm_field_group_create_mode_id returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);

    rv = bcm_field_group_action_set(unit, EXTENDER__PMF_FIELD_GRP_ID_RESTORE_ETAG, aset);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_action_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_install(unit, EXTENDER__PMF_FIELD_GRP_ID_RESTORE_ETAG);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_install\n");
        print rv;
    }

    bcm_field_group_config_t_init(&group_cfg1);

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(group_cfg1.qset);
    BCM_FIELD_QSET_ADD(group_cfg1.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_cfg1.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_cfg1.qset, bcmFieldQualifyTunnelTerminated);   /* TT Lookup 0 - E-Channel registration */
    BCM_FIELD_QSET_ADD(group_cfg1.qset, bcmFieldQualifyIpTunnelHit);        /* TT Lookup 1 - Untagged check */


    /* Define the ASET */
    BCM_FIELD_ASET_INIT(group_cfg1.aset);
    BCM_FIELD_ASET_ADD(group_cfg1.aset, bcmFieldActionDrop);

    /*  Create the Field group */
    group_cfg1.priority = 1;
    group_cfg1.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET;
    rv = bcm_field_group_config_create(unit, &group_cfg1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }

    EXTENDER__PMF_FIELD_GRP_ID_ING_VALIDATION = group_cfg1.group;

    rv = bcm_field_group_install(unit, EXTENDER__PMF_FIELD_GRP_ID_ING_VALIDATION);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    printf("Done creating field filter\n");

    return rv;
}

/* Add a PMF rule per port:
   - Ingress Validation (drop) - E-Channel registration and no Etag filter
   - Etag restore
*/
/*
 * Configuration of the extender channel registration and no etag filter.
 *
 * E-channel registration is performed using tt lookup #1 and the bcmPortControlNonEtagDrop port property configures the
 * port to use an e-tpid tt lookup #2, and this field program will filter any packets that fail their tt lookups.
 *
 * Setting the port property bcmPortControlNonEtagDrop and E-channel registration is not enough to create filtering -
 * the filtering itself is done in the field proccessor.
 * Therefore, in order to configure the filter, this function should be called, per port.
 * When the filter is removed, this field program should be removed as well.
 */
int extender__pmf_port_rules_add(int unit, int port)
{
    int rv;
    bcm_field_entry_t ent, ent1, ent2;
    int enabled;

    /* Entry 1: TT lookup 0 (E-Channel registration check) */

    rv = bcm_field_entry_create(unit, EXTENDER__PMF_FIELD_GRP_ID_ING_VALIDATION, &ent1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }
    rv = bcm_field_qualify_TunnelTerminated(unit, ent1, 0x0, 0x1);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_field_qualify_TunnelTerminated\n");
      return rv;
    }
    rv = bcm_field_qualify_InPort(unit, ent1, port, ~0);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_field_qualify_InPort\n");
      return rv;
    }
    rv = bcm_field_action_add(unit, ent1, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_action_add\n");
        return rv;
    }
    rv = bcm_field_entry_install(unit, ent1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_install returned %s\n", bcm_errmsg(rv));
    }

    /* Entry 2: TT lookup 1 (untagged check)  - only if enabled */

    rv = bcm_port_control_get(unit, port, bcmPortControlNonEtagDrop, &enabled);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_get for bcmPortControlNonEtagDrop, port - %d, rv - %d\n", port, rv);
    }

    if (enabled) {
        rv = bcm_field_entry_create(unit, EXTENDER__PMF_FIELD_GRP_ID_ING_VALIDATION, &ent2);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_create\n");
            return rv;
        }
        rv = bcm_field_qualify_IpTunnelHit(unit, ent2, 0x0, 0x1);
        if (rv != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_IpTunnelHit\n");
          return rv;
        }
        rv = bcm_field_qualify_InPort(unit, ent2, port, ~0);
        if (rv != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_InPort\n");
          return rv;
        }
        rv = bcm_field_action_add(unit, ent2, bcmFieldActionDrop, 0, 0);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_action_add\n");
            return rv;
        }
        rv = bcm_field_entry_install(unit, ent2);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_install returned %s\n", bcm_errmsg(rv));
        }
    }

    /* Restore packets Etag */
    rv = bcm_field_entry_create(unit, EXTENDER__PMF_FIELD_GRP_ID_RESTORE_ETAG, &ent);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, ent, port, 0xffffffff);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_qualify_InPort returned %s\n", bcm_errmsg(rv));
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionStartPacketStrip, bcmFieldStartToL2Strip, 0x38 /*-8 Bytes in two s complement*/);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_action_add returned %s\n", bcm_errmsg(rv));
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_install returned %s\n", bcm_errmsg(rv));
    }

    return BCM_E_NONE;
}


/* Cleanup LIF and ECID registration */
int extender__extender_port_cleanup(int unit, bcm_gport_t gport)
{
    int rv;

    rv = bcm_extender_port_delete(unit, gport);
    if (rv != BCM_E_NONE) {
        printf("\n\n >>>>>  bcm_extender_port_delete rv = 0x%x (0x%x)\n\n",rv, gport);
    }

    return rv;
}



int extender__pmf_cleanup(int unit, int group_id)
{

    int rv, i, ent_count;
    bcm_field_entry_t ent[10];


    /**************************
     *   Destroy PMF Entries  *
 */

    do {
        /* get a bunch of entries in this group */
        rv = bcm_field_entry_multi_get(unit, group_id, 10, &(ent[0]), ent_count);
        if (BCM_E_NONE != rv) {
            return rv;
        }
        for (i = 0; i < ent_count; i++) {
            /* remove each entry from hardware and destroy it */
            rv = bcm_field_entry_remove(unit, ent[i]);
            if (BCM_E_NONE != rv) {
                return rv;
            }
            rv = bcm_field_entry_destroy(unit, ent[i]);
            if (BCM_E_NONE != rv) {
                return rv;
            }
        }
    /* all as long as there were entries to remove & destroy */
    } while (ent_count > 0);


   /**************************
    *   Destroy PMF Groups   *
 */
    rv = bcm_field_group_remove(unit, group_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_remove\n");
        return rv;
    }

    rv = bcm_field_group_destroy(unit,group_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    return rv;
}



/* Clean pmf groups */
int extender__cleanup(int unit)
{
    int rv;

    rv = extender__pmf_cleanup(unit, EXTENDER__PMF_FIELD_GRP_ID_RESTORE_ETAG);
    if (BCM_E_NONE != rv) {
        printf("Error in extender__pmf_cleanup, group = %d\n", EXTENDER__PMF_FIELD_GRP_ID_RESTORE_ETAG);
        return rv;
    }

    rv = extender__pmf_cleanup(unit, EXTENDER__PMF_FIELD_GRP_ID_ING_VALIDATION);
    if (BCM_E_NONE != rv) {
        printf("Error in extender__pmf_cleanup, group = %d\n", EXTENDER__PMF_FIELD_GRP_ID_ING_VALIDATION);
        return rv;
    }

    return rv;
}
