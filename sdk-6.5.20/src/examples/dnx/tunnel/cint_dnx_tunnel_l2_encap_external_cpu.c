/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_dnx_tunnel_l2_encap_external_cpu.c
 * Purpose: Example of trapping snooping and forwarding IPv4 tunneled packets to a remote CPU.
 */
/*
 * This CINT contains an example of packet tunneling to external CPU.
 * The packet is encapsulated above the system headers with an outer LL header, according to ARP encapsulation specification.
 *
 * In order to receive remote CPU tunneling, the following configurations are required:
 * 1. soc property for the header type of the remote CPU port:
 * tm_port_header_type_out_<cpu_port>.BCM88690=ENCAP_EXTERNAL_CPU
 *
 * 2. FTMH Ethernet next protocol should be configured through bcm_switch_control_set with type=bcmSwitchFtmhEtherType
 *
 *
 * Scenarios tested in this cint:
 * Test 1:
 * forwarding to external CPU port
 * dnx_tunnel_l2_external_cpu_info_t structure indicates the encapsulation header information
 * the l2_external_cpu_info.l2_remote_arp_intf field will hold the lif index.
 *
 * preserve_trap parameter is used to indicate that in case of trap/snoop, the redirected packet will
 * contain system headers stack that includes FHEI header with trap/snoop command information.
 *
 * Example of usage in JR2 (NOTE: the exact system headers details depend on system configuration):
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/tunnel/cint_dnx_tunnel_l2_encap_external_cpu.c
 * cint
 * dnx_tunnel_l2_external_cpu_run_all(0,16,201,202,1);
 * exit;
 *
 * Ip over ethernet packet, UC forwarding and snooping
 * tx 1 psrc=16 data=0x000c000200000011000001120800450000350000000080005873a0000011a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x6789ab010a1b2c3d4e5f810070648999019c00086480000000e4af49bb0000000000000004040e00020000a00400200210000100000c000200000011000001120800450000350000000080005873a0000011a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * SA=DA, trap to remote CPU
 * tx 1 psrc=15 data=0x000c00020000000c000200000800450000350000000080005873a0000011a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x6789ab020a1b2c3d4e5f81007064899901a40007e501000023e474784900000000000000000000000200000007fa0003000000004500000c00020000000c000200000800450000350000000080005873a0000011a0a1a1a3000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * MC group destination to 2 remote encaps
 * tx 1 psrc=16 data=0x000c000200000011000001120800450000350000000080005874a0000011a0a1a1a2000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x6789ab030a1b2c3d4e5f810070648999019000086485000030e4e7c71a0000000000000004040e00020000a00400000200000c000200000011000001120800450000350000000080005874a0000011a0a1a1a2000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Source port: 0, Destination port: 0
 * Data: 0x6789ab020a1b2c3d4e5f810070648999019000086485000020e4e7c71a0000000000000004040e00020000a00400000200000c000200000011000001120800450000350000000080005874a0000011a0a1a1a2000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */
struct dnx_tunnel_l2_external_cpu_info_t
{
    /*
     * Remote encapsulation information
     */
    bcm_port_t remote_port1;
    bcm_port_t remote_port2;
    bcm_if_t l2_remote_arp_intf[3];
    bcm_mac_t l2_remote_arp_src_mac;
    bcm_mac_t l2_remote_arp_dst_mac_fwd;
    bcm_mac_t l2_remote_arp_dst_mac_trap;
    bcm_mac_t l2_remote_arp_dst_mac_snoop;
    int l2_remote_tpid;
    int l2_remote_vlan;
    int l2_remote_ethertype;
    /*
     * Incoming packet specification
     */
    bcm_port_t src_port;
    bcm_if_t intf_in;
    bcm_mac_t intf_in_mac_address;
    int vrf;
    bcm_ip_t dip_fwd;
    bcm_ip_t dip_mc_group;
    int fec_id_enc;
    bcm_multicast_t mc_group;
    /*
     * General config
     */
    uint8 preserve_trap;
    int snoop_trap_id;
};

dnx_tunnel_l2_external_cpu_info_t l2_external_cpu_info =
    /*
     * ports : out_port1 | out_port2 - filled by input
     */
{ 0, 0,
    /*
     * l2_remote_arp_intf[3] - allocated WITH_ID
     */
    {0x5671, 0x5672, 0x5673},
    /*
     * l2_remote_arp_src_mac
     */
    {0x0A, 0x1B, 0x2C, 0x3D, 0x4E, 0x5F},
    /*
     * l2_remote_arp_dst_mac_fwd | l2_remote_arp_dst_mac_trap | l2_remote_arp_dst_mac_snoop
     */
    {0x23, 0x45, 0x67, 0x89, 0xAB, 0x01}, {0x23, 0x45, 0x67, 0x89, 0xAB, 0x02}, {0x23, 0x45, 0x67, 0x89, 0xAB, 0x03},
    /*
     * l2_remote_tpid
     */
    0x8100,
    /*
     * l2_remote_vlan
     */
    100,
    /*
     * l2_remote_ethertype
     */
    0x8999,
    /*
     * src_port - filled by input
     */
    0,
    /*
     * intf_in
     */
    20,
    /*
     * intf_in_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /*
     * vrf
     */
    1,
    /*
     * dip_fwd |  dip_mc_group
     */
    0xA0A1A1A3 /* 160.161.161.163 */ , 0xA0A1A1A2 /* 160.161.161.162 */ ,
    /*
     * fec_id_enc
     */
    0x2000C351,
    /*
     * mc_group
     */
    111,
    /*
     * preserve_trap
     */
    1
};

struct cint_ipv4_tunnel_info_s
{
    int tunnel_fec_id;                    /* FEC id */
    bcm_ip_t     tunnel_ipv4_sip;         /* Tunnel SIP */
    bcm_ip_t     tunnel_ipv4_dip;         /* Tunnel DIP */
    bcm_tunnel_type_t tunnel_type;        /* tunnel type */
    int tunnel_ttl;                       /* Ip tunnel header TTL */
    int tunnel_dscp;                      /* Ip tunnel header differentiated services code point */
};

cint_ipv4_tunnel_info_s cint_ipv4_tunnel_info =
{

        /*
         * tunnel_fec_id
         */
        0x2000C351,
        /*
         * tunnel_ipv4_sip
         */
        0xA0000013, /* 160.0.0.19 */
        /*
         * tunnel_ipv4_dip
         */
        0xA1000011, /* 161.0.0.17 */
        /*
         * tunnel type
         */
        bcmTunnelTypeIpAnyIn4,
        /*
         * tunnel_ttl
         */
        64,
        /*
         * tunnel_dscp
         */
        10
};

/*
 * Snoop any packet that is going through src_port to remote CPU
 */
int
l2_external_cpu_snoop_by_src_port(
    int unit)
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
    bcm_field_group_t cint_in_port_qaul_fg_id = 0;
    bcm_gport_t in_port_gport;
    bcm_field_entry_t ent1_id;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_gport_t gport_tunnel_temp;      /* mirror header info works with GPORTs */
    int rv = BCM_E_NONE;


    /*
     * Create a new snoop command to be associated with the new trap and OutLif
     */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = l2_external_cpu_info.remote_port1;
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;
    /* Add trap FHEI preserve */
    mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;

    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_create.\n");
        return rv;
    }
    /*
     * Set mirror header info (keep original headers if required)
     */
    flags = BCM_MIRROR_DEST_IS_SNOOP;
    if (l2_external_cpu_info.preserve_trap)
    {
        flags |= BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER;
    }

    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_tunnel_temp, l2_external_cpu_info.l2_remote_arp_intf[2]);
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.out_vport = gport_tunnel_temp;
    mirror_header_info.tm.src_sysport = l2_external_cpu_info.src_port;
    rv = bcm_mirror_header_info_set(unit, flags, mirror_dest.mirror_dest_id, &mirror_header_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_header_info_set.\n");
        return rv;
    }

    /*
     * Check if snoop was created
     */
    is_snoop = BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id);
    if (!is_snoop)
    {
        printf("Error. Could not create snoop.\n");
        return -1;
    }

    /*
     * Get the snoop command
     */
    snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);

    /*
     * Create a new trap
     */
    flags = 0;
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_create.\n");
        return rv;
    }
    l2_external_cpu_info.snoop_trap_id = trap_code;
    /*
     * Setup the new trap
     */
    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd;

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_set.\n");
        return rv;
    }

    /*
     * Encode trap, trap strength and snoop strength as a gport
     */
    BCM_GPORT_TRAP_SET(snoop_trap_gport_id, trap_code, trap_strength, snoop_strength);

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
    rv = bcm_field_group_context_attach(unit, 0, cint_in_port_qaul_fg_id, 0 , &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_LOCAL_SET(in_port_gport, l2_external_cpu_info.src_port);
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
 * Init remark profile to be later used for outer LL entry
 */
int
arp_plus_ac_qos(
    int unit,
    int pcp,
    int dei,
    int *qos_egress)
{
    bcm_error_t rv;
    int opcode_egress;
    bcm_qos_map_t l2_eg_map;
    int flags;

    /*
     * Create Egress Remark profile which is used by VLAN-Port (or ARP VLAN translation)
     */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_create(unit, flags, qos_egress);
    if (rv != BCM_E_NONE)
    {
        printf("error in L3 egress bcm_qos_map_create\n");
    }

    /*
     * Create Egress opcode which is later used to set the required pcp mapping
     */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_create(unit, flags, &opcode_egress);
    if (rv != BCM_E_NONE)
    {
        printf("error in egress l3 opcode bcm_qos_map_create\n");
        return rv;
    }

    /*
     * Clear structure
     */
    bcm_qos_map_t_init(&l2_eg_map);

    /*
     * Map Egress Remark profile to L2 Opcode
     */
    l2_eg_map.opcode = opcode_egress;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l2_eg_map, *qos_egress);
    if (rv != BCM_E_NONE)
    {
        printf("error in egress l2 opcode bcm_qos_map_add\n");
        return rv;
    }
    /*
     * Set L2 opcode mappings
     */
    int idx;
    for (idx = 0; idx < 256; idx++)
    {
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK;

        bcm_qos_map_t_init(&l2_eg_map);
        /*
         * Input internal priority NWK-QOS
         */
        l2_eg_map.int_pri = idx;
        /*
         * Input color
         */
        l2_eg_map.color = 0;

        /*
         * Set egress PCP and DEI
         */
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
int
create_rcpu_tunnel(
    int unit,
    int tunnel_index,
    int qos_egress)
{
    bcm_error_t rv;
    int eth_out_rif;
    int flags;
    int arp_outlif;
    bcm_gport_t arp_ac_gport;
    bcm_mac_t dst_mac;

    eth_out_rif = 100 + tunnel_index;

    /*
     * Create Eth rif for outer Ethernet SRC MAC
     */
    rv = intf_eth_rif_create(unit, eth_out_rif, l2_external_cpu_info.l2_remote_arp_src_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * Create ARP+AC entry for outer Ethernet and set its properties
     * ARP+AC indication is flag BCM_L3_FLAGS2_VLAN_TRANSLATION
     */
    switch (tunnel_index)
    {
        case 0:
            dst_mac = l2_external_cpu_info.l2_remote_arp_dst_mac_fwd;
            break;
        case 1:
            dst_mac = l2_external_cpu_info.l2_remote_arp_dst_mac_trap;
            break;
        case 2:
            dst_mac = l2_external_cpu_info.l2_remote_arp_dst_mac_snoop;
            break;
    }
    rv = l3__egress_only_encap__create_inner(unit, BCM_L3_WITH_ID, &l2_external_cpu_info.l2_remote_arp_intf[tunnel_index], dst_mac,
                                             eth_out_rif, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION);
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
    arp_outlif = BCM_L3_ITF_VAL_GET(l2_external_cpu_info.l2_remote_arp_intf[tunnel_index]);
    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_ac_gport, arp_outlif);
    BCM_GPORT_VLAN_PORT_ID_SET(arp_ac_gport, arp_ac_gport);

    /*
     * Connect ARP VLAN translation to VLAN-Translation Command 3 which always add VLAN
     */
    int vlan_edit_profile = 3;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.gport = arp_ac_gport;
    port_trans.vlan_edit_class_id = vlan_edit_profile;
    port_trans.new_outer_vlan = l2_external_cpu_info.l2_remote_vlan;
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
    action.outer_tpid = l2_external_cpu_info.l2_remote_tpid;
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
    action_class.tag_format_class_id = 0;       /* Untagged */
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /*
     * Connect ARP VLAN translation to Remark profile which is used to decide the PCP value on VLAN tag
     */
    rv = bcm_qos_port_map_set(unit, arp_ac_gport, -1, qos_egress);
    if (rv != BCM_E_NONE)
    {
        printf("error in L2 egress bcm_qos_port_map_set");
        return rv;
    }

    return rv;
}

/*
 * Configure RCPU encapsulation
 */
int
l2_external_cpu_setup(
    int unit)
{
    int i;
    int pcp = 3;
    int dei = 1;
    bcm_error_t rv;
    int qos_egress;

    /*
     * FTMH Ethertype is device configuration
     */
    rv = bcm_switch_control_set(unit, bcmSwitchFtmhEtherType, l2_external_cpu_info.l2_remote_ethertype);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set with type=bcmSwitchFTMHEthertype\n");
        return rv;
    }

    /*
     * Create Eth rif for outer Ethernet SRC MAC
     */
    rv = arp_plus_ac_qos(unit, pcp, dei, &qos_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    for (i = 0; i < 3; ++i)
    {
        rv = create_rcpu_tunnel(unit, i, qos_egress);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create_rcpu_tunnel\n");
            return rv;
        }

    }

    return BCM_E_NONE;

}

/*
 * Add ip forwarding into remote CPU
 */
int
l2_external_cpu_uc_forwarding(
    int unit)
{

    bcm_error_t rv;

    /*
     * Create l3 forwarding entry for the mc_group
     */
    rv = add_host_ipv4(unit, l2_external_cpu_info.dip_fwd, l2_external_cpu_info.vrf, 0,
                       BCM_L3_ITF_VAL_GET(l2_external_cpu_info.l2_remote_arp_intf[0]),
                       l2_external_cpu_info.remote_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_route, \n");
        return rv;
    }
    printf("Configured ipv4 forwarding to port: %d, encap: %d\n", l2_external_cpu_info.remote_port1, BCM_L3_ITF_VAL_GET(l2_external_cpu_info.l2_remote_arp_intf[0]));

    return BCM_E_NONE;

}

/*
 * Trap bcmRxTrapLinkLayerSaEqualsDa packets to remote CPU
 */
int
l2_external_cpu_uc_trap_set(
    int unit)
{

    bcm_error_t rv;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int trap_id = 0;

    sal_memset(&config, 0, sizeof(config));

    /*
     * for port dest change
     */
    config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_ENCAP_ID;
    if (l2_external_cpu_info.preserve_trap)
    {
        config.flags |= BCM_RX_TRAP_TRAP;
    }

    config.trap_strength = 7;
    config.dest_port = l2_external_cpu_info.remote_port2;

    bcm_gport_t gport_tunnel_temp;      /* rx trap works with GPORTs */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_tunnel_temp, l2_external_cpu_info.l2_remote_arp_intf[1]);
    config.encap_id = gport_tunnel_temp;

    config.snoop_cmnd = 0;
    config.snoop_strength = 0;

    flags = 0;
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapLinkLayerSaEqualsDa, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id %d \n", trap_id);
        return rv;
    }

    rv = bcm_rx_trap_set(unit, trap_id, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in trap set \n");
        return rv;
    }

    printf("Configured trap to port: %d, encap: %d\n", l2_external_cpu_info.remote_port2, BCM_L3_ITF_VAL_GET(l2_external_cpu_info.l2_remote_arp_intf[1]));

    return BCM_E_NONE;

}

/*
 * Add ip forwarding into MC group of 2 RCPU members
 */
int
l2_external_cpu_mc(
    int unit)
{

    bcm_error_t rv;

    int flags;
    flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    rv = bcm_multicast_destroy(unit, l2_external_cpu_info.mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Warning, in mc destroy, mc_group $l2_external_cpu_info.mc_group \n");
    }
    rv = bcm_multicast_create(unit, flags, &l2_external_cpu_info.mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mc create, flags $flags mc_group $l2_external_cpu_info.mc_group \n");
        return rv;
    }

    printf("mc group created %d \n", l2_external_cpu_info.mc_group);

    bcm_gport_t gport;
    BCM_GPORT_LOCAL_SET(gport, l2_external_cpu_info.remote_port1);

    bcm_multicast_replication_t replications[2];
    replications[0].encap1 = BCM_L3_ITF_VAL_GET(l2_external_cpu_info.l2_remote_arp_intf[1]);
    replications[0].port = gport;
    replications[1].encap1 = BCM_L3_ITF_VAL_GET(l2_external_cpu_info.l2_remote_arp_intf[2]);
    replications[1].port = gport;
    rv = bcm_multicast_add(unit, l2_external_cpu_info.mc_group, BCM_MULTICAST_INGRESS_GROUP, 2, &replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_add\n");
        return rv;
    }

    /*
     * Configure fec entry for encapsulation flow
     */
    bcm_gport_t mc_gport;
    BCM_GPORT_MCAST_SET(mc_gport, l2_external_cpu_info.mc_group);
    rv = l3__egress_only_fec__create(unit, l2_external_cpu_info.fec_id_enc, 0, 0, mc_gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * Create l3 forwarding entry for the mc_group
     */
    rv = add_host_ipv4(unit, l2_external_cpu_info.dip_mc_group, l2_external_cpu_info.vrf,
                       l2_external_cpu_info.fec_id_enc, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_route, \n");
        return rv;
    }

    printf("Configured ipv4 mc group to port: %d, encaps: %d and %d\n", l2_external_cpu_info.remote_port1,
            BCM_L3_ITF_VAL_GET(l2_external_cpu_info.l2_remote_arp_intf[1]),
            BCM_L3_ITF_VAL_GET(l2_external_cpu_info.l2_remote_arp_intf[2]));

    return BCM_E_NONE;

}


/*
 * ip forwarding basic configuration
 */
int
ip_forwarding_ingress_configuration(
    int unit)
{
    int rv = BCM_E_NONE;

    /*
     * Configure port properties for this application
     */
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, l2_external_cpu_info.src_port, l2_external_cpu_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    /*
     * Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, l2_external_cpu_info.intf_in, l2_external_cpu_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = l2_external_cpu_info.intf_in;
    ingr_itf.vrf = l2_external_cpu_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    return rv;
}

/*
 * Create ipv4 tunnel
 * Create FEC and add ipv4 host entry
 */
int
l2_external_cpu_tunnel_create(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int fec_id = 0;
    bcm_l3_intf_t l3_intf;
    bcm_tunnel_initiator_t tunnel_init;

    bcm_l3_intf_t_init(&l3_intf);

    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = cint_ipv4_tunnel_info.tunnel_ipv4_dip;
    tunnel_init.sip = cint_ipv4_tunnel_info.tunnel_ipv4_sip;
    tunnel_init.flags = 0;
    tunnel_init.dscp = cint_ipv4_tunnel_info.tunnel_dscp;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type = cint_ipv4_tunnel_info.tunnel_type;
    tunnel_init.encap_access = bcmEncapAccessTunnel2;
    tunnel_init.l3_intf_id = l2_external_cpu_info.l2_remote_arp_intf[0];
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init);

    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create\n");
        return rv;
    }

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ipv4_tunnel_info.tunnel_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /*
     * Create FEC
     */
    rv = l3__egress_only_fec__create(unit, cint_ipv4_tunnel_info.tunnel_fec_id, l3_intf.l3a_intf_id, 0, l2_external_cpu_info.remote_port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }

    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ipv4_tunnel_info.tunnel_fec_id);
    /*
     * Add Ipv4 host entry
     */
    rv = add_host_ipv4(unit, l2_external_cpu_info.dip_fwd,l2_external_cpu_info.vrf, fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv4\n");
        return rv;
    }

    return rv;
}

/*
 * main function to create all forward-trap-snoop scenarios combined.
 * preserve_trap parameter is used to indicate that in case of trap/snoop, the redirected packet will
 * contain system headers stack that includes FHEI header with trap/snoop command information.
 */
int
dnx_tunnel_l2_external_cpu_run_all(
    int unit,
    bcm_port_t src_port,
    bcm_port_t out_port1,
    bcm_port_t out_port2,
    uint8 preserve_trap,
    uint8 create_ipv4_tunnel)
{

    int rv = BCM_E_NONE;

    l2_external_cpu_info.src_port = src_port;
    l2_external_cpu_info.remote_port1 = out_port1;
    l2_external_cpu_info.remote_port2 = out_port2;
    l2_external_cpu_info.preserve_trap = preserve_trap;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &l2_external_cpu_info.fec_id_enc);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /*
     * Configure the incoming packet to do IP forwarding
     */
    rv = ip_forwarding_ingress_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_forwarding_configuration \n");
        print rv;
        return rv;
    }

    /*
     * Configure remote cpu encapsulation
     */
    printf("l2_external_cpu_setup\n");
    rv = l2_external_cpu_setup(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l2_external_cpu_setup \n");
        print rv;
        return rv;
    }

    if (create_ipv4_tunnel == 1)
    {
      printf("l2_external_cpu_tunnel_create\n");
      rv = l2_external_cpu_tunnel_create(unit);
      if (rv != BCM_E_NONE)
      {
          printf("Error, l2_external_cpu_tunnel_create \n");
          print rv;
          return rv;
      }
    }

    if (!create_ipv4_tunnel)
    {
       /*
        * Set ip uc forwarding to l2_remote_arp_intf[0] & remote_port1
        */
        printf("l2_external_cpu_uc_forwarding\n");
        rv = l2_external_cpu_uc_forwarding(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, l2_external_cpu_uc_forwarding \n");
            print rv;
            return rv;
        }
    }

    /*
     * Set snooping according to src port to l2_remote_arp_intf[2] & remote_port1
     */
    rv = l2_external_cpu_snoop_by_src_port(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l2_external_cpu_snoop_by_src_port \n");
        print rv;
        return rv;
    }

    /*
     * Set trapping according to SA=DA to l2_remote_arp_intf[1] & remote_port2
     */
    printf("l2_external_cpu_uc_trap_set\n");
    rv = l2_external_cpu_uc_trap_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l2_external_cpu_uc_trap_set \n");
        print rv;
        return rv;
    }
    if (!create_ipv4_tunnel)
    {
      /*
      * Set ip forwarding to MC group of l2_remote_arp_intf[1], l2_remote_arp_intf[2] & remote_port1
      */
      printf("l2_external_cpu_mc_forwarding\n");
      rv = l2_external_cpu_mc(unit);
      if (rv != BCM_E_NONE)
      {
          printf("Error, l2_external_cpu_mc_forwarding \n");
          print rv;
          return rv;
      }
    }
    return rv;

}
