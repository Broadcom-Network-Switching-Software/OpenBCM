/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.SDK/src/examples/sand/cint cint_l2_encap_external_cpu.c
 * 
 * configuration
 * BCM> print l2_external_cpu_setup(unit, cpu_port1, cpu_port2)
 * 
 * execution:
 * * BCM>cint
 * cint>l2_external_cpu_uc_forwarding(unit);
 * cint>exit;
 * for uc send
 * BCM> tx 1 DATA=0x0000000000eb0000000000138100000108990123456789098765432123456789c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 PSRC=13
 * the added l2 header will be 23456789abcd0a1b2c3d4e5f810050648999+system headers
 * 
 * for uc ingress trap
 * execution:
 * BCM>cint
 * cint>l2_external_cpu_uc_trap_set(unit);
 * cint>exit;
 * for uc send (trap for same DA and SA)
 * BCM> tx 1 DATA=0x0000000000eb00x0000000000eb8100000108990123456789098765432123456789c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 PSRC=13
 * the added l2 header will be 23456789abcd0a1b2c3d4e5f810050648999+system headers
 *
 * 
 * for mc send
 * BCM>cint
 * cint>l2_external_cpu_mc_forwarding(unit);
 * cint>exit;
 * BCM> tx 1 DATA=0xff00000000eb0000000000138100000108990123456789098765432123456789c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 PSRC=13
 * the added l2 header will be 23456789abcd0a1b2c3d4e5f810050648999+system headers
 */
struct l2_external_cpu_l2_cpu_config_t
{
    bcm_port_encap_config_t encap_config[3];
    bcm_tunnel_initiator_t l2_encap_tunnel[3];
    bcm_l3_intf_t tunnel_info[3];
    bcm_port_t port_list[2];
    int num_of_cpu_ports;
};

l2_external_cpu_l2_cpu_config_t l2_cpu_config;

/*
 * Snoop any packet that is going through src_port to remote CPU
 * This sequence is for JR2 devices
 */
int l2_external_cpu_snoop_by_src_port_j2(int unit, bcm_port_t src_port)
{
    bcm_mirror_destination_t mirror_dest;
    bcm_rx_trap_config_t trap_config_snoop;
    int is_snoop;
    int snoop_cmnd;
    int trap_code;
    int flags = 0;
    int trap_strength = 0;
    int snoop_strength = 3;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    void *dest_char;
    bcm_gport_t snoop_trap_gport_id;
    bcm_field_group_t   cint_in_port_qaul_fg_id = 0;
    bcm_gport_t in_port_gport;
    bcm_field_entry_t ent1_id;

    int rv = BCM_E_NONE;

    /* Create a new snoop command to be associated with the new trap and OutLif */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = l2_cpu_config.port_list[0];
    mirror_dest.encap_id = BCM_L3_ITF_VAL_GET(l2_cpu_config.tunnel_info[2].l3a_intf_id); /*outlif (not a gport)*/
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP | BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;

    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_create.\n");
        return rv;
    }

    /* Check if snoop was created */
    is_snoop = BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id);
    if (!is_snoop)
    {
        printf("Error. Could not create snoop.\n");
        return -1;
    }

    /* Get the snoop command */
    snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);

    /* Create a new trap */
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_type_create.\n");
        return rv;
    }

    /* Setup the new trap */
    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd;

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_set.\n");
        return rv;
    }


    /* Encode trap, trap strength and snoop strength as a gport */
    BCM_GPORT_TRAP_SET(snoop_trap_gport_id, trap_code, trap_strength, snoop_strength);
    print src_port;
    print snoop_trap_gport_id;

    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionSnoop;

    printf("Creating first group\n");
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "In_port_qual", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_in_port_qaul_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = 1;
    attach_info.key_info.qual_types[0] = bcmFieldQualifyInPort;
    attach_info.payload_info.nof_actions = 1;
    attach_info.payload_info.action_types[0] = bcmFieldActionSnoop;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_in_port_qaul_fg_id, 0/*context_id?*/, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_LOCAL_SET(in_port_gport,src_port);
    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = bcmFieldQualifyInPort;
    ent_info.entry_qual[0].value[0] = in_port_gport;
    ent_info.entry_qual[0].mask[0] = 0x1ff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionSnoop;
    ent_info.entry_action[0].value[0] = snoop_trap_gport_id;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_in_port_qaul_fg_id, &ent_info, &ent1_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Snoop any packet that is going through src_port to remote CPU
 * This sequence is for JR1 devices
 */
int l2_external_cpu_snoop_by_src_port(int unit, bcm_port_t src_port)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_set_t presel_set;
    bcm_field_presel_t presel_id;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_config_t fg;
    bcm_field_group_t fg_group = 2;
    bcm_field_entry_t entry_1;
    bcm_port_t src_port_mask = 0xffffffff;
    int flags = 0;                            
    int snoop_command;                      /* Snoop command */
    bcm_rx_snoop_config_t snoop_config;
    int trap_id;
    int trap_dest_strength = 0;             /* No influence on the destination update */         
    int trap_snoop_strength = 3;            /* Strongest snoop strength for this trap */
    bcm_rx_trap_config_t trap_config;       /* Snoop attributes */ 
    bcm_gport_t snoop_trap_gport_id;

    bcm_port_t dest_port=l2_cpu_config.port_list[0];
    int encap_id=l2_cpu_config.tunnel_info[2].l3a_intf_id;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
    int advanced_mode=soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE);

    /*  Create Snoop command */

    rv = bcm_rx_snoop_create(unit, flags, &snoop_command);
    if (rv != BCM_E_NONE) 
    {
        printf("Error in bcm_rx_snoop_create\n");
        return rv;
    }

    bcm_rx_snoop_config_t_init(&snoop_config);                      /* Initialize the structure */

    snoop_config.flags |= BCM_RX_SNOOP_UPDATE_DEST|BCM_RX_SNOOP_UPDATE_ENCAP_ID;    /* Define Destination flag for snoop */
    BCM_GPORT_LOCAL_SET(snoop_config.dest_port, dest_port);                  /* Set the snoop destination to dest */

    snoop_config.encap_id=encap_id;
    snoop_config.size = -1;                                         /* Full packet snooping */
    snoop_config.probability = 100000;                              /* Set probability to 100% */
    rv = bcm_rx_snoop_set(unit, snoop_command, &snoop_config);  /* Set snoop configuration */
    if (rv != BCM_E_NONE) 
    {
        printf("Error in bcm_rx_snoop_set\n");
        return rv;
    }

    /*  Create a User-defined trap for snooping */

    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error in bcm_rx_trap_type_create\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);                        /* Configure the trap to the snoop command */ 
    trap_config.flags |= BCM_RX_TRAP_REPLACE;                       /* Set flag for trap - replace */
    trap_config.trap_strength = trap_dest_strength;                 /* Set trap strength */
    trap_config.snoop_cmnd = snoop_command;                         /* set trap snoop command */
    trap_config.snoop_strength = trap_snoop_strength; 
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) 
    {
        printf("Error in bcm_rx_trap_set\n");
        return rv;
    }

    BCM_GPORT_TRAP_SET(snoop_trap_gport_id, trap_id, trap_dest_strength, trap_snoop_strength); /* Get the trap gport to snoop */

    /*  Create preselector with qualifer src Port */

    bcm_field_presel_set_t_init(&presel_set);
    if (advanced_mode) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        presel_id = 4;
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            bcm_field_presel_destroy(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS);
            return rv;
        }
    } else {
        rv = bcm_field_presel_create(unit,&presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            bcm_field_presel_destroy(unit, presel_id);
            return rv;
        }
    }

    rv = bcm_field_qualify_InPort(unit, presel_id | presel_flags, src_port, src_port_mask);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_qualify_InPort\n");
        return rv;
    }

    /*  Add preselector to preselector set */

    BCM_FIELD_PRESEL_ADD(presel_set,presel_id);

    /*  Create qset - create a key with qualifer in port */

    BCM_FIELD_QSET_INIT(&qset);
    BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInPort);

    /*  Create aset - snoop action result from lookup */

    BCM_FIELD_ASET_INIT(&aset);
    BCM_FIELD_ASET_ADD(aset,bcmFieldActionSnoop);

    /*  Define a database (Field-Group) and associate it with a pre-selector, QSET and ASET */

    bcm_field_group_config_t_init(&fg);
    fg.group = fg_group;
    fg.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
    fg.qset = qset;
    fg.priority = 2;
    fg.mode = bcmFieldGroupModeAuto;
    fg.aset = aset;
    fg.preselset = presel_set;

    bcm_field_group_config_create(unit, &fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_config_create\n");
        return rv;
    }

    /*  Create entry and associate it with field group */

    rv = bcm_field_entry_create(unit,fg.group,&entry_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_entry_create\n");
        return rv;
    }

    bcm_field_qualify_InPort(unit,entry_1,src_port,src_port_mask);

    bcm_field_action_add(unit, entry_1, bcmFieldActionSnoop, snoop_trap_gport_id, 0);

    /*  Write the entry to the HW database - commit all changes to the hardware */

    bcm_field_group_install(unit,fg.group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_install\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * create RCPU tunnel
 * Relevant for JR1 device only
 */
int create_rcpu_tunnel_jr1(int unit, int tunnel_index)
{
    bcm_error_t rv;

    bcm_tunnel_initiator_t_init(&l2_cpu_config.l2_encap_tunnel[tunnel_index]);
    bcm_l3_intf_t_init(&l2_cpu_config.tunnel_info[tunnel_index]);
    /* span info */
    l2_cpu_config.l2_encap_tunnel[tunnel_index].type = bcmTunnelTypeL2EncapExternalCpu;


    rv = bcm_tunnel_initiator_create(unit, &l2_cpu_config.tunnel_info[tunnel_index] , &l2_cpu_config.l2_encap_tunnel[tunnel_index]);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    printf("Allocated tunnel id:%x\n",l2_cpu_config.l2_encap_tunnel[tunnel_index].tunnel_id);

    rv = bcm_port_encap_config_set(unit,l2_cpu_config.l2_encap_tunnel[tunnel_index].tunnel_id,&(l2_cpu_config.encap_config[tunnel_index]));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_encap_config_set \n");
        return rv;
    }

    return rv;
}

/*
 * Init remark profile to be later used for outer LL entry
 */
int arp_plus_ac_qos_jr2(int unit, int pcp, int dei, int * qos_egress)
{
    bcm_error_t rv;
    int opcode_egress;
    bcm_qos_map_t l2_eg_map;
    int flags;

    /* Create Egress Remark profile which is used by VLAN-Port (or ARP VLAN translation) */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_create(unit, flags, qos_egress);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_create\n");
    }

    /* Create Egress opcode which is later used to set the required pcp mapping */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE ;
    rv = bcm_qos_map_create(unit, flags, &opcode_egress);
    if (rv != BCM_E_NONE) {
        printf("error in egress l3 opcode bcm_qos_map_create\n");
        return rv;
    }

    /* Clear structure */
    bcm_qos_map_t_init(&l2_eg_map);

    /* Map Egress Remark profile to L2 Opcode */
    l2_eg_map.opcode = opcode_egress;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l2_eg_map, *qos_egress);
    if (rv != BCM_E_NONE) {
        printf("error in egress l2 opcode bcm_qos_map_add\n");
        return rv;
    }
    /* Set L2 opcode mappings */
    int idx;
    for (idx = 0; idx < 256; idx++)
    {
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK;

        bcm_qos_map_t_init(&l2_eg_map);
        /* Input internal priority NWK-QOS */
        l2_eg_map.int_pri = idx;
        /* Input color */
        l2_eg_map.color = 0;

        /* Set egress PCP and DEI */
        l2_eg_map.pkt_pri = pcp;
        l2_eg_map.pkt_cfi = dei;

        rv = bcm_qos_map_add(unit, flags, &l2_eg_map, opcode_egress);
        if (rv != BCM_E_NONE)
        {
            printf("error in L2 egress bcm_qos_map_add profile(0x%08x)\n", opcode_egress);
            return rv;
        }
    }

    return rv;
}

/*
 * create RCPU tunnel and vlan editing
 * Relevant for JR2 device only
 * In JR2 RCPU tunnel is done by Routing processing of Intf and ARP+AC object.
 */
int create_rcpu_tunnel_jr2(int unit, int tunnel_index, int qos_egress)
{
    bcm_error_t rv;
    int eth_out_rif ;
    int flags;
    int arp_outlif;
    bcm_gport_t arp_ac_gport;

    eth_out_rif = 100+tunnel_index;

    /*
     * Create Eth rif for outer Ethernet SRC MAC
     */
    rv = intf_eth_rif_create(unit, eth_out_rif, l2_cpu_config.encap_config[tunnel_index].src_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * Create ARP+AC entry for outer Ethernet and set its properties
     * ARP+AC indication is flag BCM_L3_FLAGS2_VLAN_TRANSLATION
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &l2_cpu_config.tunnel_info[tunnel_index].l3a_intf_id,
            l2_cpu_config.encap_config[tunnel_index].dst_mac, eth_out_rif, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * for arp+ac, bcm_vlan_port_translation_set is the API which sets the VLAN editing part in
     * the ARP+AC entry in EEDB.
     * The following part it used to configure vlan and QOS editing on the outer LL
     */
    arp_outlif = BCM_L3_ITF_VAL_GET(l2_cpu_config.tunnel_info[tunnel_index].l3a_intf_id);
    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_ac_gport, arp_outlif);
    BCM_GPORT_VLAN_PORT_ID_SET(arp_ac_gport, arp_ac_gport);

    /* Connect ARP VLAN translation to VLAN-Translation Command 3 which always add VLAN */
    int vlan_edit_profile = 3;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.gport = arp_ac_gport;
    port_trans.vlan_edit_class_id = vlan_edit_profile;
    port_trans.new_outer_vlan = l2_cpu_config.encap_config[tunnel_index].vlan;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("error in bcm_vlan_port_translation_set");
        return rv;
    }

    /*
     * Create action ID
     */
    int action_id_1;
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Set translation action
     */
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = l2_cpu_config.encap_config[tunnel_index].tpid;
    action.dt_outer = bcmVlanActionArpVlanTranslateAdd;
    action.dt_outer_pkt_prio = bcmVlanActionAdd;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id_1, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /*
     * Set translation action class (map edit_profile & tag_format to action_id)
     */
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = vlan_edit_profile;
    action_class.tag_format_class_id = 0; /* Untagged */
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /* Connect ARP VLAN translation to Remark profile which is used to decide the PCP value on VLAN tag */
    rv = bcm_qos_port_map_set(unit, arp_ac_gport, -1, qos_egress);
    if (rv != BCM_E_NONE)
    {
        printf("error in L2 egress bcm_qos_port_map_set");
        return rv;
    }

    return rv;
}

/* create encapsulation information*/
int l2_external_cpu_setup(int unit, int num_of_cpu_ports, bcm_port_t out_port1, bcm_port_t out_port2)
{
    int i;
    int pcp=3;
    int dei=1;
    bcm_error_t rv;
    int ethertype = 0x8999;
    int qos_egress;

    l2_cpu_config.port_list[0]=out_port1;

    if (num_of_cpu_ports==2) {
        l2_cpu_config.port_list[1]=out_port2;
    }
    else
    {
        l2_cpu_config.port_list[1]=out_port1;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * FTMH Ethertype is device configuration
         */
        rv = bcm_switch_control_set(unit, bcmSwitchFtmhEtherType, ethertype);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_control_set with type=bcmSwitchFTMHEthertype\n");
            return rv;
        }

        /*
         * Create Eth rif for outer Ethernet SRC MAC
         */
        rv = arp_plus_ac_qos_jr2(unit, pcp, dei, &qos_egress);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create intf_out\n");
            return rv;
        }
    }

    for (i=0;i<3;++i) {
        l2_cpu_config.encap_config[i].encap = BCM_PORT_ENCAP_IEEE;
        l2_cpu_config.encap_config[i].dst_mac[0]=0x23;
        l2_cpu_config.encap_config[i].dst_mac[1]=0x45;
        l2_cpu_config.encap_config[i].dst_mac[2]=0x67;
        l2_cpu_config.encap_config[i].dst_mac[3]=0x89;
        l2_cpu_config.encap_config[i].dst_mac[4]=0xAB;
        l2_cpu_config.encap_config[i].dst_mac[5]=i;

        l2_cpu_config.encap_config[i].src_mac[0]=0x0A;
        l2_cpu_config.encap_config[i].src_mac[1]=0x1B;
        l2_cpu_config.encap_config[i].src_mac[2]=0x2C;
        l2_cpu_config.encap_config[i].src_mac[3]=0x3D;
        l2_cpu_config.encap_config[i].src_mac[4]=0x4E;
        l2_cpu_config.encap_config[i].src_mac[5]=0x5F;


        l2_cpu_config.encap_config[i].tpid=0x8100;
        l2_cpu_config.encap_config[i].vlan=100;
        l2_cpu_config.encap_config[i].oui_ethertype=ethertype;


        l2_cpu_config.encap_config[i].tos=pcp<<1|dei;

        if (is_device_or_above(unit, JERICHO2))
        {
            rv = create_rcpu_tunnel_jr2(unit, i, qos_egress);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create_rcpu_tunnel_jr2\n");
                return rv;
            }
        }

        else
        {
            rv = create_rcpu_tunnel_jr1(unit, i);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create_rcpu_tunnel_jr1\n");
                return rv;
            }
        }

    }

    return BCM_E_NONE;

}



int l2_external_cpu_uc_forwarding(int unit) {

    bcm_error_t rv;
    uint8 fwd_mac[6] = {0x00,0x00,0x00,0x00,0x00,0xeb}; 

    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr,fwd_mac,1);

    l2_addr.vid=1;
    l2_addr.port=l2_cpu_config.port_list[0];
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.encap_id = BCM_L3_ITF_VAL_GET(l2_cpu_config.tunnel_info[0].l3a_intf_id); /*outlif (not a gport)*/

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add\n");
        return rv;
    }

    return BCM_E_NONE;

}

int l2_external_cpu_uc_trap_set(int unit) {

    bcm_error_t rv;
    bcm_rx_trap_config_t config;
    int flags=0;
    int trap_id=0;

    sal_memset(&config, 0, sizeof(config));

    /*for port dest change*/
    config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_ENCAP_ID;

    config.trap_strength = 7; /*FOR USER DEFINE ONLY !!! */
    config.dest_port=l2_cpu_config.port_list[1];
    if (is_device_or_above(unit, JERICHO2))
    {
        bcm_gport_t gport_tunnel_temp; /* rx trap works with GPORTs */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_tunnel_temp, l2_cpu_config.tunnel_info[1].l3a_intf_id);
        config.encap_id=gport_tunnel_temp;
    }
    else
    {
        config.encap_id=l2_cpu_config.tunnel_info[1].l3a_intf_id;
    }

    config.snoop_cmnd = 0;
    config.snoop_strength = 0;

    /*bcmRxTrapSaEqualsDa or bcmRxTrapLinkLayerSaEqualsDa in JR2 */
    if (is_device_or_above(unit, JERICHO2))
    {	
        rv = bcm_rx_trap_type_create(unit,flags,bcmRxTrapLinkLayerSaEqualsDa,&trap_id);
    }
    else
    {
        rv = bcm_rx_trap_type_create(unit,flags,bcmRxTrapSaEqualsDa,&trap_id);
    }
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap id %d \n",trap_id);
        return rv;
    }


    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    return BCM_E_NONE;

}

int l2_external_cpu_mc_forwarding(int unit) {

    bcm_error_t rv;
    uint8 fwd_mac[6] = {0xff,0x00,0x00,0x00,0x00,0xeb}; 
    bcm_l2_addr_t l2_addr;
    int i;

    bcm_multicast_t mc_group = 111;
    int flags;
    flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID ;
    rv = bcm_multicast_destroy(unit, mc_group);
    if (rv != BCM_E_NONE) {
        printf("Warning, in mc destroy,  mc_group $mc_group \n");
    }
    rv = bcm_multicast_create(unit, flags, &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in mc create, flags $flags mc_group $mc_group \n");
        return rv;
    }

    printf("mc group created %d, \n",mc_group);


    bcm_l2_addr_t_init(&l2_addr,fwd_mac,1);

    l2_addr.vid=1;
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mc_group;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }

    bcm_gport_t gport;
    BCM_GPORT_LOCAL_SET(gport, l2_cpu_config.port_list[0]); 

    if (is_device_or_above(unit, JERICHO2)) {
        bcm_multicast_replication_t replications[2];
        replications[0].encap1 = BCM_L3_ITF_VAL_GET(l2_cpu_config.tunnel_info[1].l3a_intf_id);
        replications[0].port = gport;
        replications[1].encap1 = BCM_L3_ITF_VAL_GET(l2_cpu_config.tunnel_info[2].l3a_intf_id);
        replications[1].port = gport;
        rv = bcm_multicast_add(unit, mc_group, BCM_MULTICAST_INGRESS_GROUP, 2, &replications);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_add\n");
            return rv;
        }
    }
    else {
        rv = bcm_multicast_ingress_add(unit, mc_group, gport, l2_cpu_config.tunnel_info[1].l3a_intf_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in mc ingress add, mc_group $mc_group gport $gport \n");
            return rv;
        }

        rv = bcm_multicast_ingress_add(unit, mc_group, gport, l2_cpu_config.tunnel_info[2].l3a_intf_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in mc ingress add, mc_group $mc_group gport $gport \n");
            return rv;
        }
    }

    return BCM_E_NONE;

}

int l2_external_cpu_sanitiy_check(int unit) {

    bcm_error_t rv;
    bcm_port_config_t encap_config;
    bcm_tunnel_initiator_t l2_encap_tunnel;
    bcm_l3_intf_t tunnel_info;
    int i;

    for (i=0;i<100;++i) {
        bcm_tunnel_initiator_t_init(&l2_encap_tunnel);
        bcm_l3_intf_t_init(&tunnel_info);
        l2_encap_tunnel.type       = bcmTunnelTypeL2EncapExternalCpu;


        rv = bcm_tunnel_initiator_create(unit, &tunnel_info , &l2_encap_tunnel);
        if(rv != BCM_E_NONE) {
            printf("Error, create tunnel initiator \n");
            return rv;
        }


        rv = bcm_tunnel_initiator_clear(unit, &tunnel_info);
        if(rv != BCM_E_NONE) {
            printf("Error, create tunnel initiator \n");
            return rv;
        }
    }

    bcm_tunnel_initiator_t_init(&l2_encap_tunnel);
    bcm_l3_intf_t_init(&tunnel_info);
    /* span info */
    l2_encap_tunnel.type       = bcmTunnelTypeL2EncapExternalCpu;

    l2_encap_tunnel.tunnel_id=0x0f000;
    l2_encap_tunnel.flags|=BCM_TUNNEL_WITH_ID;

    rv = bcm_tunnel_initiator_create(unit, &tunnel_info , &l2_encap_tunnel);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    rv = bcm_tunnel_initiator_clear(unit, &tunnel_info);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    return BCM_E_NONE;
}


int l2_external_cpu_run_all(int unit, int cpu_ports, bcm_port_t src_port, bcm_port_t out_port1, bcm_port_t out_port2) {

    int rv=BCM_E_NONE;

    printf("l2_external_cpu_setup\n");
    rv = l2_external_cpu_setup(unit, cpu_ports,out_port1, out_port2);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_external_cpu_setup \n");
        print rv;
        return rv;
    }

    if (!is_device_or_above(unit, JERICHO2))
    {
        printf("l2_external_cpu_sanitiy_check\n");
        rv = l2_external_cpu_sanitiy_check(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, l2_external_cpu_sanitiy_check \n");
            print rv;
            return rv;
        }
    }

    printf("l2_external_cpu_uc_forwarding\n");
    rv = l2_external_cpu_uc_forwarding(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_external_cpu_uc_forwarding \n");
        print rv;
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = l2_external_cpu_snoop_by_src_port_j2(unit, src_port);
        if (rv != BCM_E_NONE) {
            printf("Error, l2_external_cpu_snoop_by_src_port \n");
            print rv;
            return rv;
        }
    }
    else
    {
        rv = l2_external_cpu_snoop_by_src_port(unit, src_port);
        if (rv != BCM_E_NONE) {
            printf("Error, l2_external_cpu_snoop_by_src_port \n");
            print rv;
            return rv;
        }
    }

    printf("l2_external_cpu_uc_trap_set\n");
    rv = l2_external_cpu_uc_trap_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_external_cpu_uc_trap_set \n");
        print rv;
        return rv;
    }


    printf("l2_external_cpu_mc_forwarding\n");
    rv = l2_external_cpu_mc_forwarding(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_external_cpu_mc_forwarding \n");
        print rv;
        return rv;
    }

    return rv;

}

