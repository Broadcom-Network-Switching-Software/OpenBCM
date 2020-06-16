/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_ip_snoop.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * bcm_port_t in_port = 200;
 * int dest_local_port = 1;
 * int udp_port_min = 100;
 * int udp_port_max = 200;
 * cint_field_ip_snoop_main(unit, context_id, in_port, dest_local_port, udp_port_min, udp_port_max);
 *
 * Configuration example end
 */

/*   
 * Snoop IP-routed packets  
 *  
 *  Sequence:
 *  1. Insert an entry to match in the IP Host table
 *  2. Define a Field Group match IP packets that are routed
 *  (match in the IP Host table) with specific UDP port range.
 *  We are qualifying only upon L4 Destination port, because
 *  L4 Source port range value is set to cover full range.
 *  3. Create the Field group and add entries. Snoop the packet
 *  and modify the internal priority if the frame is matched
 */


/*
 *  IMPORTANT NOTE:
 *  The configuration of the Destination Host Table is done
 *  with another CINT:
 *  cint "cint_ip_route.c"
 */


bcm_field_group_t cint_field_ip_snoop_fg_id =0;
bcm_gport_t snoop_trap_gport_id;
int trap_id;
bcm_field_entry_t cint_field_ip_snoop_ent_id;
bcm_mirror_destination_t mirror_dest;


/*
 *  Set a Snoop profile to send to some destination
 *  local port. (Alternatively, a system port can be the
 *  destination of the mirror port).
 *  We receive as an output the snoop gport. This gport
 *  will be the parameter of snoop action  
 */
int cint_field_ip_snoop_set(/* in */ int unit,
                            /* in */ int dest_local_port,
                            /* in */ int update_fabric_header_editing, /* When set, FHEI will be added to snooped copy*/
                            /* in */ int set_outlif, /* When set, outlif will be added to the snooped copy*/
                            /* in */ int Outlif,
                            /* out */ bcm_gport_t *snoop_trap_gport_id,
                            /* out */ int * mirror_dest_id)
{

    int auxRes;
    int flags = 0; /* Do not specify any ID for the snoop command / trap */
    int snoop_command; /* Snoop command */
    int trap_dest_strength = 0; /* No influence on the destination update */
    int trap_snoop_strength = 7; /* Strongest snoop strength for this trap */
    bcm_rx_snoop_config_t snoop_config; /* Snoop attributes */
    bcm_rx_trap_config_t trap_config;
    int rv;

    if (NULL == snoop_trap_gport_id)
    {
        printf("Pointer to Snoop gport ID must not be NULL\n");
        return BCM_E_PARAM;
    }

    /* Initialize a mirror destination structure */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags |= BCM_MIRROR_DEST_IS_SNOOP;
    if (update_fabric_header_editing)
    {
        mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    }
    /* set the gport id of mirror port */
    BCM_GPORT_LOCAL_SET(mirror_dest.gport, dest_local_port);

    if(set_outlif)
    {
         mirror_dest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID ;
         mirror_dest.encap_id = Outlif;
    }

    /* create a mirror destination */
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_mirror_destination_create $rv\n");
        return rv;
    }

    /* return mirror dest id*/
    *mirror_dest_id = mirror_dest.mirror_dest_id;
    /*
         * Create a User-defined trap for snooping
         */
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_create\n");
        auxRes = bcm_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
        auxRes = bcm_rx_trap_type_destroy(unit, trap_id);
        return rv;
    }

    /*
    * Configure the trap to the snoop command
    */
    bcm_rx_trap_config_t_init(&trap_config);

    /*for port dest change*/
    trap_config.trap_strength = trap_dest_strength;
    trap_config.snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id); /* Snoop any frame matched by this trap */

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_set\n");
        auxRes = bcm_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
        auxRes = bcm_rx_trap_type_destroy(unit, trap_id);
        return rv;
    }

    /* Get the trap gport to snoop */
    BCM_GPORT_TRAP_SET(snoop_trap_gport_id, trap_id, trap_dest_strength, trap_snoop_strength);


    printf("created gport trap=%d\n", *snoop_trap_gport_id);

    return rv;
}


/*
 *  Set the Field group that:
 *  - Match UDPoIPoEth packets, routed by the IP Host table,
 *  and whose UDP Port is in a specific range
 *  - If match, snoop the packet and modify its Traffic Class  
 */
int cint_field_ip_snoop_fg_set(/* in */ int unit,
	                         /* in */ bcm_field_context_t context_id,
                             /* in */ bcm_port_t in_port,
                             /* in */ int udp_port_min,
                             /* in */ int udp_port_max,
                             /* in */ bcm_gport_t snoop_trap_gport_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_t ip_snoop_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_range_info_t range_info;
    bcm_field_range_t range_id = 0;
    bcm_gport_t in_port_gport;
    void *dest_char;
    int rv = BCM_E_NONE;

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

    /** Define the L4 Destination Port range. */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeL4DstPort;
    range_info.min_val = udp_port_min;
    range_info.max_val = udp_port_max;
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, range_id, &range_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_range_set\n",  rv);
       return rv;
    }

    /**
     * Define the L4 Source Port range to be with full range [0-65535],
     * cause we don't care about it, and qualifying upon L4 Destination Port.
     */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeL4SrcPort;
    range_info.min_val = 0;
    range_info.max_val = 0xffff;
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, range_id, &range_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_range_set\n",  rv);
       return rv;
    }

    /*
     * Create and attach TCAM group in iPMF1
	*/
	 bcm_field_group_info_t_init(&fg_info);
	fg_info.fg_type = bcmFieldGroupTypeTcam;
	fg_info.stage = bcmFieldStageIngressPMF1;
	fg_info.nof_quals = 3;
	fg_info.qual_types[0] = bcmFieldQualifyInPort;
	fg_info.qual_types[1] = bcmFieldQualifyIp4Protocol;
	fg_info.qual_types[2] = bcmFieldQualifyL4PortRangeCheck;
	fg_info.nof_actions = 2;
	fg_info.action_types[0] = bcmFieldActionSnoop;
	fg_info.action_types[1] = bcmFieldActionPrioIntNew;

	printf("Creating first group\n");
	dest_char = &(fg_info.name[0]);
	sal_strncpy_s(dest_char, "IP snoop", sizeof(fg_info.name));
	rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_ip_snoop_fg_id);
	if (rv != BCM_E_NONE)
	{
		printf("Error (%d), in bcm_field_group_add for IP snoop\n", rv);
	    return rv;
	}

	printf("Attaching IP snoop to the created context...\r\n");

	bcm_field_group_attach_info_t_init(&attach_info);

	attach_info.key_info.nof_quals = fg_info.nof_quals;
	attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
	attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
	attach_info.key_info.qual_types[2] = fg_info.qual_types[2];


	attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
	attach_info.key_info.qual_info[0].input_arg = 0;
	attach_info.key_info.qual_info[0].offset = 0;


	attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
	attach_info.key_info.qual_info[1].input_arg = 1;
	attach_info.key_info.qual_info[1].offset = 0;

	attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
	attach_info.key_info.qual_info[2].input_arg = 0;
	attach_info.key_info.qual_info[2].offset = 0;

	attach_info.payload_info.nof_actions = fg_info.nof_actions;
	attach_info.payload_info.action_types[0] = fg_info.action_types[0];
	attach_info.payload_info.action_types[1] = fg_info.action_types[1];

	rv = bcm_field_group_context_attach(unit, 0, cint_field_ip_snoop_fg_id, context_id, &attach_info);
	if (rv != BCM_E_NONE)
	{
		printf("Error (%d), in bcm_field_group_context_attach\n", rv);
		return rv;
	}

    /*
    *  Add multiple entries to the Field group.
    * 
    *  Match the packets on the EtherType = 0x0800 (IPv4 packets),
    *  Ip4Protocol = 0x11 (UDP), Range of the UDP Destination Ports,
    *  and Host-Destination table hit
    */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_LOCAL_SET(in_port_gport,in_port);
    ent_info.nof_entry_quals = 3;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = in_port_gport;
    ent_info.entry_qual[0].mask[0] = 0x1ff;

    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = 0x11;
    ent_info.entry_qual[1].mask[0] = 0xFFFF;
    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = (1 << range_id);
    ent_info.entry_qual[2].mask[0] = (1 << range_id);

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = snoop_trap_gport_id;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 7;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_field_ip_snoop_fg_id, &ent_info, &cint_field_ip_snoop_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}


/*
 *  Set the previous configurations for an example configuration
 *  If match by a Field entry, snoop the packet to in_port
 */
int cint_field_ip_snoop_main(int unit,bcm_field_context_t context_id, bcm_port_t in_port, int dest_local_port, int udp_port_min, int udp_port_max)
{
    int rv;
    int mirror_dest_id_dummy;

    /* Set the snoop destination */
    rv = cint_field_ip_snoop_set(unit, dest_local_port,0, 0, 0, &snoop_trap_gport_id, &mirror_dest_id_dummy);
    if (BCM_E_NONE != rv) {
        printf("Error in cint_field_ip_snoop_set\n");
        return rv;
    }

    rv = cint_field_ip_snoop_fg_set(unit, context_id, in_port, udp_port_min, udp_port_max, snoop_trap_gport_id);
    if (BCM_E_NONE != rv) {
        printf("Error in cint_field_ip_snoop_fg_set\n");
        return rv;
    }

    return rv;
}



int cint_field_ip_snoop_destroy(int unit, bcm_field_context_t context_id)
{
    int rv;
    bcm_rx_trap_config_t trap_config;

    /* Destroy the snoop destination */
    rv = bcm_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_rx_snoop_destroy\n");
        return rv;
    }

    /* Destroy the User-Defined trap type */
    rv = bcm_rx_trap_type_destroy(unit, trap_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_rx_trap_type_destroy\n");
        return rv;
    }

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_field_ip_snoop_fg_id, NULL, cint_field_ip_snoop_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_field_ip_snoop_ent_id, cint_field_ip_snoop_fg_id);
        return rv;
    }

    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_field_ip_snoop_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_field_ip_snoop_fg_id, context_id);
        return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_field_ip_snoop_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_field_ip_snoop_fg_id);
        return rv;
    }


    return rv;
}
