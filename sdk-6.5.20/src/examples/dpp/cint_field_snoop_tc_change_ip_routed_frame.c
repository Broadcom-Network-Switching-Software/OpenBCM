
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*   
 * Snoop IP-routed packets  
 *  
 *  Sequence:
 *  1. Insert an entry to match in the IP Host table
 *  2. Define a Field Group match IP packets that are routed
 *  (match in the IP Host table) with specific UDP port range
 *  3. Create the Field group and add entries. Snoop the packet
 *  and modify the internal priority if the frame is matched
 */


/*
 *  IMPORTANT NOTE:
 *  The configuration of the Destination Host Table is done
 *  with another CINT:
 *  cint "cint_ip_route.c"
 */


bcm_field_group_t group;
bcm_gport_t snoop_trap_gport_id;
int trap_id;


/*
 *  Set a Snoop profile to send to some destination
 *  local port. (Alternatively, a system port can be the
 *  destination of the mirror port).
 *  We receive as an output the snoop gport. This gport
 *  will be the parameter of snoop action  
 */
int ip_snoop_set(/* in */ int unit,
                 /* in */ int dest_local_port,
                 /* out */ bcm_gport_t *snoop_trap_gport_id) 
{
    int result;
    int auxRes;
    int flags = 0; /* Do not specify any ID for the snoop command / trap */
    int snoop_command; /* Snoop command */
    int trap_dest_strength = 0; /* No influence on the destination update */
    int trap_snoop_strength = 3; /* Strongest snoop strength for this trap */
    bcm_rx_snoop_config_t snoop_config; /* Snoop attributes */
    bcm_rx_trap_config_t trap_config;

    if (NULL == snoop_trap_gport_id) {
        printf("Pointer to Snoop gport ID must not be NULL\n");
        return BCM_E_PARAM;
    }

    /* 
    * Create a Snoop Command 
    * The snoop command defines the attribute of the Snoop 
    */ 
    result = bcm_rx_snoop_create(unit, flags, &snoop_command); /* Receive a snoop command handle */
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_snoop_create\n");
        auxRes = bcm_rx_snoop_destroy(unit, flags, snoop_command);
        return result;
    }

    /* 
    * Specify the snoop attributes
    * Here, we set the destination and the probability to 100% 
    * Snoop the whole packet 
    */
    /* Initialize the structure */
    bcm_rx_snoop_config_t_init(&snoop_config);
    snoop_config.flags |= BCM_RX_SNOOP_UPDATE_DEST;

    /* Set the Mirror destination to go to the Destination local port*/
    BCM_GPORT_LOCAL_SET(snoop_config.dest_port, dest_local_port);
    snoop_config.size = -1; /* Full packet snooping */
    snoop_config.probability= 100000; /* 100000 is 100.000% */

    /*Set snoop configuration*/
    result = bcm_rx_snoop_set(unit, snoop_command, &snoop_config);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_rx_snoop_set\n");
        auxRes = bcm_rx_snoop_destroy(unit, flags, snoop_command);
        return result;
    }

    /* 
    * Create a User-defined trap for snooping
    */
    result = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_id);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_type_create\n");
        auxRes = bcm_rx_snoop_destroy(unit, flags, snoop_command);
        auxRes = bcm_rx_trap_type_destroy(unit, trap_id);
        return result;
    }

    /* 
    * Configure the trap to the snoop command 
    */ 
    bcm_rx_trap_config_t_init(&trap_config);
  
    /*for port dest change*/
    trap_config.flags |= BCM_RX_TRAP_REPLACE; 
    trap_config.trap_strength = trap_dest_strength; 
    trap_config.snoop_cmnd = snoop_command; /* Snoop any frame matched by this trap */
    trap_config.snoop_strength = trap_snoop_strength; 

    result = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_set\n");
        auxRes = bcm_rx_snoop_destroy(unit, flags, snoop_command);
        auxRes = bcm_rx_trap_type_destroy(unit, trap_id);
        return result;
    }

    /* Get the trap gport to snoop */
    BCM_GPORT_TRAP_SET(snoop_trap_gport_id, trap_id, trap_dest_strength, trap_snoop_strength);

    return result;
}


/*
 *  Set the Field group that:
 *  - Match UDPoIPoEth packets, routed by the IP Host table,
 *  and whose UDP Port is in a specific range
 *  - If match, snoop the packet and modify its Traffic Class  
 */
int ip_snoop_field_group_set(/* in */ int unit,
                             /* in */ int group_priority,
                             /* in */ bcm_port_t in_port,
                             /* in */ int udp_port_min,
                             /* in */ int udp_port_max,
                             /* in */ bcm_gport_t snoop_trap_gport_id,
                             /* out */ bcm_field_group_t *group) 
{
    int result;
    int auxRes;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    bcm_field_range_t range;
  

    if (NULL == group) {
        printf("Pointer to group ID must not be NULL\n");
        return BCM_E_PARAM;
    }

    /*
    *  Build qualifier set for the group we want to create.  Entries must exist
    *  within groups, and groups define what kind of things the entries can
    *  match, by their inclusion in the group's qualitier set.
    * 
    *  The Field group is done at the ingress stage
    *  The Field group qualifiers are EtherType (to match only IPv4 packets),
    *  IP-Protocol (UDP as L4), L4-Port range, and
    *  hit in the Destination Host Table for specific Port. 
    */

    /* 
    * Define the L4 Destination Port range 
    */
    result = bcm_field_range_create(unit,
                                    &range,
                                    BCM_FIELD_RANGE_UDP | BCM_FIELD_RANGE_DSTPORT,
                                    udp_port_min /* min port number */,
                                    udp_port_max /* max port number */);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_range_create\n");
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }
  
    /* Define the QSET */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL3DestHostHit);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4PortRangeCheck);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    /*
    *  Build the action set for this Field group.  Entries can only do
    *  something that is included on this list.
    * 
    *  This Field Group can perform Snooping and change the Internal
    *  priority
    */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);

    /*  Create the Field group */
    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create\n");
        auxRes = bcm_field_range_destroy(unit, range);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_range_destroy(unit, range);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    /*
    *  Add multiple entries to the Field group.
    * 
    *  Match the packets on the EtherType = 0x0800 (IPv4 packets),
    *  IpProtocol = 0x11 (UDP), Range of the UDP Destination Ports,
    *  and Host-Destination table hit
    */
    result = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }

    /* IPv4 EtherType */
    result = bcm_field_qualify_EtherType(unit, ent, 0x0800, 0xFFFF);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_EtherType\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }

    /* UDP as Layer 4 */
    result = bcm_field_qualify_IpProtocol(unit, ent, 0x11, 0xFFFF);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_IpProtocol\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }

    /* Destination Host table NOT hit (1 bit) */
    result = bcm_field_qualify_L3DestHostHit(unit, ent, 0x1, 0x1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_L3DestHostHit\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }

    /* In-Port according to the input */
    result = bcm_field_qualify_InPort(unit, ent, in_port, -1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InPort\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }

    /* Range of the UDP Destination Ports*/
    result = bcm_field_qualify_L4PortRangeCheck(unit, ent, range, FALSE);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_RangeCheck\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }


    /*
    *  A matched frame is snooped to the defined gport.
    */
    result = bcm_field_action_add(unit, ent, bcmFieldActionSnoop, snoop_trap_gport_id, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }

    /*
    *  Change the Traffic Class of a matched frame to 7
    */
    result = bcm_field_action_add(unit, ent, bcmFieldActionPrioIntNew, 7, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        auxRes = bcm_field_range_destroy(unit, range);
        return result;
    }

    /*
    *  Commit the entire group to hardware.  We could do individual entries,
    *  but since we just created the group here and want the entry/entries all
    *  in hardware at this point, it is quicker to commit the whole group.
    */
    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    /*
    *  Everything went well; return the group ID that we allocated earlier.
    */
    *group = grp;

    return result;
}


/*
 *  Set the previous configurations for an example configuration
 *  If match by a Field entry, snoop the packet to port 1
 */
int ip_snoop_example(int unit) 
{
    int result;
    int dest_local_port = 1;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
    int udp_port_min = 100;
    int udp_port_max = 200;
    bcm_port_t in_port = 1;


    /* Set the snoop destination */
    result = ip_snoop_set(unit, dest_local_port, &snoop_trap_gport_id); 
    if (BCM_E_NONE != result) {
        printf("Error in ip_snoop_set\n");
        return result;
    }

    result = ip_snoop_field_group_set(unit, group_priority,
                                      in_port,
                                      udp_port_min, udp_port_max,
                                      snoop_trap_gport_id, &group);
    if (BCM_E_NONE != result) {
        printf("Error in ip_field_group_set\n");
        return result;
    }

    return result;
}


/*
 *  Set the previous configurations for an example configuration
 *  If match by a Field entry, snoop the packet to in_port
 */
int ip_snoop_main(int unit, bcm_port_t in_port, int dest_local_port, int udp_port_min, int udp_port_max) 
{
    int result;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;

    /* Set the snoop destination */
    result = ip_snoop_set(unit, dest_local_port, &snoop_trap_gport_id); 
    if (BCM_E_NONE != result) {
        printf("Error in ip_snoop_set\n");
        return result;
    }

    result = ip_snoop_field_group_set(unit, group_priority, in_port, udp_port_min, udp_port_max, snoop_trap_gport_id, &group);
    if (BCM_E_NONE != result) {
        printf("Error in ip_field_group_set\n");
        return result;
    }

    return result;
}



int ip_snoop_teardown(int unit) 
{
    int result;
    bcm_rx_trap_config_t trap_config;

    /* Destroy the snoop destination */

    /* Get the trap id and its atributes */
    trap_id = BCM_GPORT_TRAP_GET_ID(snoop_trap_gport_id);
    result = bcm_rx_trap_get(unit, trap_id, &trap_config);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_trap_get\n");
        return result;
    }

    /* Destroy the snoop command */
    result = bcm_rx_snoop_destroy(unit, 0, trap_config.snoop_cmnd);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_snoop_destroy\n");
        return result;
    }

    /* Destroy the User-Defined trap type */
    result = bcm_rx_trap_type_destroy(unit, trap_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_trap_type_destroy\n");
        return result;
    }

    /* Destroy the Field group (and all its entries) */  
    result = bcm_field_group_flush(unit, group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_flush\n");
        return result;
    }

    return result;
}
