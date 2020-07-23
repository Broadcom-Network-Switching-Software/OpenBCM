/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple-router scenario 
 * Test Scenario 
 *
 * ./bcm.user
 * cint sand/utility/cint_sand_utils_global.c
 * cint sand/utility/cint_sand_utils_tpid.c
 * cint sand/utility/cint_sand_utils_vlan_translate.c
 * cint dnx/field/cint_field_utils.c
 * cint sand/cint_ip_route_basic.c
 * cint dnx/dbal/cint_dbal.c
 * cint dnx/cint_dnx_mirror_example.c
 * cint dnx/crps/cint_crps_oam_config.c
 *
 * Tests:
 *
 * 1. SPAN
 *         cint
 *         mirror_span_example(0, 200, 201, 202);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201/202:
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2. RSPAN
 *         cint
 *         mirror_rspan_example(0, 200, 15, 201, 202, 100);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201/202:
 *             000000000002000000000001810000648100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 3. ERSPANv2
 *         cint
 *         mirror_erspan_example_v2(0, 200, 15, 201, 202);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201:
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *         expect to receive packet from port 202:
 *             000c0002000000123456789a810000640800450a00a0000000003c2f3d03a0000011a1000011100088be0000000010640000000a00c8
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 4. ERSPANv3
 *         cint
 *         mirror_erspan_example_v3(0, 200, 15, 201, 202);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201:
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *         expect to receive packet from port 202:
 *             000c0002000000123456789a810000640800450a00ac000000003c2f3cf7a0000011a1000011100088be00000000106400001b791d00000083a5165a00c800000079
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * 5. ERSPAN with SN
 *         cint
 *         erspan_info.gre_with_sn=1;
 *         mirror_erspan_example_v2(0, 200, 15, 201, 202);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201:
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *         expect to receive packet from port 202 (Sequence Number in GRE header increments for each injected packet):
 *             000c0002000000123456789a810000640800450a00ac000000003c2f3cf7a0000011a1000011100088be00000000106400001b791d00000083a5165a00c800000079
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

/*
 * Structure to describe an ERSPAN tunnel.
 */
struct erspan_tunnel_info_t
{
    /* if TRUE ERSPAN works in v3 mode */
    int is_erspan_v3;

    /* fields common for ERSPANv2 & v3 header */
    int vlan;
    int cos; /* 3 bits */
    int truncated; /* 1 bit */
    int direction; /* 1 bit */
    int session_id; /* 10 bits */

    /* 'EN' bits in ERSPANv2 and 'BSO' bits in ERSPANv3 */
    int en; /* 2 bits */

    /* fields for ERSPANv2 only */
    int index_17_19; /* 3 bits */

    /* fields for ERSPANv3 only */
    int hw_id; /* 6 bits */
    int switch_id; /* 10 bits */

    /* IPv4 tunnel */
    bcm_ip_t dip;
    bcm_ip_t sip;
    int dscp;
    int ttl;

    /* Link layer */
    bcm_mac_t next_hop;
    bcm_vlan_t vid;

    /* IPv6 tunnel */
    bcm_ip6_t dip6;
    bcm_ip6_t sip6;

    /* Sequence Number */
    int gre_with_sn;
    int counter_command_id;
    int counter_engine_id;
    int database_id;

    /* Saved info for clean-up */
    bcm_l3_intf_t rspan_intf;
    bcm_l3_intf_t erspan_intf;
    bcm_l3_intf_t ip_tunnel_intf;
    bcm_if_t next_hop_if;
};

erspan_tunnel_info_t erspan_info = {
/** is_erspan_v3   vlan   cos   truncated   direction   session_id */
    FALSE,        100,  7,     0,         0,      160, 
/** en   index_17_19   hw_id   switch_id      dip                  sip   */
     0,       5,      0x3A,   0x25A, 0xA1000011, 0xA0000011, 
/** dscp   ttl                            next_hop                              vid   */
    10,  60, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},   100,
/**     dip6   */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
/**     sip6   */
    { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 },
/** gre_with_sn counter_command_id   counter_engine_id database_id   */
    0,          0,                   0,                0
};

/*
 * Structure to describe mirror forwarding info
 */
struct mirror_info_t
{
    /* Link layer */
    bcm_mac_t da;
    bcm_vlan_t vlan;
    /* out_vlan for routed packets, equal to intf_out in dnx_basic_example_inner */
    bcm_vlan_t out_vlan;

    int eve_profile;
    int eve_action_id;

    uint32 egress_sample_rate_dividend;
    uint32 egress_sample_rate_divisor;

    /* Saved info for clean-up */
    bcm_port_tag_format_class_t tag_format_class_id;
    bcm_gport_t mirror_dest_id;
    bcm_gport_t vlan_port_id;
    int in_port;
    int out_port;
    int is_mc;
    int mc_id;
};

mirror_info_t mirror_info = {
/**                   da                   vlan  out_vlan eve_profile */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},   15,    100,       8,
/** egress_sample_rate_dividend egress_sample_rate_divisor */
             0,                              0
};

/*
 * Structure to describe PMF field group to support egress mirror probability sampling
 */
struct mirror_egress_probability_sample_t
{
    bcm_mac_t da;
    bcm_vlan_t vlan;
    int in_port;
    int out_port;

    bcm_gport_t mirror_dest_id;
    int mirror_port;

    /** mirror code offset in iPMF1 */
    int mirror_code_ipmf1_offset;
    /** mirror code offset in iPMF3 */
    int mirror_code_ipmf3_offset;
    /** number of bits to be taken from TOD LSBs to compare */
    uint32 compare_bits;

    /** context ID for mirror recycle port in iPMF1 & 2 stage*/
    bcm_field_context_t ipmf1_context_id;
    /** presel ID for mirror recycle port in iPMF1 & 2 stage */
    bcm_field_presel_t ipmf1_presel_id;

    /** context ID for mirror recycle port in iPMF3 stage */
    bcm_field_context_t ipmf3_context_id;
    /** presel ID for mirror recycle port in iPMF3 stage */
    bcm_field_presel_t ipmf3_presel_id;
    /**
    * Context selection profile for mirror RCY port.
    * PP port CS profile is 3 bits.
    */
    uint32 rcy_cs_profile;

    /** data qual ID for PP port key GEN var */
    bcm_field_qualify_t pp_port_key_gen_qual_id;
    /** data qual ID for egress TOD */
    bcm_field_qualify_t egress_tod_qual_id;
    /** data qual ID for mirror code */
    bcm_field_qualify_t mirror_code_qual_id;

    /** field group for probability sampling */
    bcm_field_group_t prob_sample_fg_id;
    bcm_field_entry_t prob_sample_ent;
    /** field group for strip header */
    bcm_field_group_t strip_header_fg_id;
    bcm_field_entry_t strip_header_ent;
};

mirror_egress_probability_sample_t mirror_prob_sample_info = {
/** da                                    vlan in_port out_port */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, 15,  0,      0,
/** mirror_dest_id  mirror_port */
    0,              0,
/** mirror_code_ipmf1_offset, mirror_code_ipmf3_offset */
    2717,                     1098,
/** compare_bits  ipmf1_context_id ipmf1_presel_id */
    8,            0,               10,
/** ipmf3_context_id ipmf3_presel_id rcy_cs_profile */
    0,               10,             1
};

/* debug information log level */
int verbose = 2;

/* Current mirroring mode */
int G_mirror_type = BCM_MIRROR_PORT_INGRESS;

/* mirroring IP tunnel type: 0-IPv4; 1-IPv6 */
int G_mirror_tunnel_is_ipv6 = 0;

/*
 * Set mirroring mode:
 * - outbound: 0 - inbound; 1 - outbound
 */
int set_outbound_mode(int unit, int outbound){
    if(outbound) {
        G_mirror_type = BCM_MIRROR_PORT_EGRESS;
    } else {
        G_mirror_type = BCM_MIRROR_PORT_INGRESS;
    }
    return 0;
}

 /*
* Set mirroring IP tunnel type:
* - is_v6: 0 - IPv4; 1 - IPv6
*/
int mirror_set_ip_tunnel_type(int unit, int is_v6){
    G_mirror_tunnel_is_ipv6 = is_v6;

    return 0;
}

 /*
* System is in JR2 system headers mode
*/
int mirror_is_jr2_system_header_mode(int unit){
    uint32 system_headers_mode;
    uint32 system_headers_mode_jr2;

    system_headers_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
    system_headers_mode_jr2 = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode_jericho2", NULL));

    return (system_headers_mode == system_headers_mode_jr2);
}

 /*
  * SPAN mirroring example. Mirrors packets received on in_port (inbound mirroring) or
  * transmitted out from out_port (outbound mirroring) to mirror destination port.
  *
  * - in_port : port that packets to be recieved from
  * - out_port : port that packets to be transmitted out
  * - mirror_port : mirror destination port
  */
int mirror_span_example(int unit, int in_port, int out_port, int mirror_port) {
    bcm_mirror_destination_t dest;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int rv;

    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;

    bcm_vlan_create(unit, mirror_info.vlan);    

    if (!BCM_GPORT_IS_TRUNK(in_port) && !BCM_GPORT_IS_TRUNK(out_port)) {
        BCM_PBMP_PORT_SET(p, in_port);
        BCM_PBMP_PORT_ADD(p, out_port);
        BCM_PBMP_CLEAR(u);
        rv = bcm_vlan_port_add(unit, mirror_info.vlan, p, u);
    } else {
        rv = bcm_vlan_gport_add(unit, mirror_info.vlan, in_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add(vlan=%d)\n", vid);
            return rv;
        }

        rv = bcm_vlan_gport_add(unit, mirror_info.vlan, out_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add(vlan=%d)\n", vid);
            return rv;
        }
    }

    l2addr.mac = mirror_info.da;
    l2addr.vid = mirror_info.vlan;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);

    bcm_mirror_destination_t_init(&dest);
    dest.gport = mirror_port;
    dest.egress_sample_rate_dividend = mirror_info.egress_sample_rate_dividend;
    dest.egress_sample_rate_divisor = mirror_info.egress_sample_rate_divisor;

    /* create the destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("Error: bcm_mirror_destination_create %d\n", rv);
        return rv;
    }

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port, G_mirror_type, dest.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_add %d\n", rv);
        return rv;
    }

    mirror_info.mirror_dest_id = dest.mirror_dest_id;

    return rv;
}
 
 /*
 * Clean up procedure for mirror_span_example
 */
int mirror_span_example_clean_up(int unit) {
    bcm_pbmp_t p;
    int rv;


    if (rv = bcm_mirror_port_dest_delete(unit, 
            (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? mirror_info.in_port : mirror_info.out_port, 
            G_mirror_type, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (rv=bcm_l2_addr_delete(unit, mirror_info.da, mirror_info.vlan)) {
        printf("Error: bcm_l2_addr_delete %d\n", rv);
    }

    if (!BCM_GPORT_IS_TRUNK(mirror_info.in_port) && !BCM_GPORT_IS_TRUNK(mirror_info.out_port)) {
        BCM_PBMP_PORT_SET(p, mirror_info.in_port);
        BCM_PBMP_PORT_ADD(p, mirror_info.out_port);

        if (rv=bcm_vlan_port_remove(unit, mirror_info.vlan, p)) {
            printf("Error: bcm_vlan_port_remove %d\n", rv);
        }
    } else {
        rv = bcm_vlan_gport_delete(unit, mirror_info.vlan, mirror_info.in_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_delete(vlan=%d)\n", vid);
            return rv;
        }

        rv = bcm_vlan_gport_delete(unit, mirror_info.vlan, mirror_info.out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_delete(vlan=%d)\n", vid);
            return rv;
        }
    }

    if (rv=bcm_vlan_destroy(unit, mirror_info.vlan)) {
        printf("Error: bcm_vlan_destroy %d\n", rv);
    }

    return rv;
}

/*
 * RSPAN mirroring example. Mirrors packets received on in_port (inbound mirroring) or
 * transmitted out from out_port (outbound mirroring) to mirror destination port with VLAN tunnel.
 *
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - rspan_tag : VLAN tag of the RSPAN tunnel
 */
int mirror_rspan_example(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int rspan_tag) {
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_vlan_port_translation_t port_trans;
    bcm_mirror_destination_t dest;
    bcm_vlan_port_t vlan_port;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int action_id_1;
    int index;
    int rv;

    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;
    mirror_info.vlan = in_vlan;

    bcm_vlan_create(unit, mirror_info.vlan);    

    if (!BCM_GPORT_IS_TRUNK(in_port) && !BCM_GPORT_IS_TRUNK(out_port)) {
        BCM_PBMP_PORT_SET(p, in_port);
        BCM_PBMP_PORT_ADD(p, out_port);
        BCM_PBMP_CLEAR(u);
        rv = bcm_vlan_port_add(unit, mirror_info.vlan, p, u);
    } else {
        rv = bcm_vlan_gport_add(unit, mirror_info.vlan, in_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add(vlan=%d)\n", vid);
            return rv;
        }

        rv = bcm_vlan_gport_add(unit, mirror_info.vlan, out_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add(vlan=%d)\n", vid);
            return rv;
        }
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    if (rv = bcm_vlan_port_create(unit, &vlan_port)) {
        printf("Error: bcm_vlan_port_create %d\n", rv);
        return rv;
    }

    mirror_info.vlan_port_id = vlan_port.vlan_port_id;

    /*
    * set edit profile for ingress/egress LIF 
    */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = rspan_tag;
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = mirror_info.eve_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_port_translation_set %d\n", rv);
        return rv;
    }

    /*
    * Create action ID
    */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_translate_action_id_create %d\n", rv);
        return rv;
    }

    mirror_info.eve_action_id = action_id_1;

    /*
    * Set translation action 
    */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = 0x8100;
    action.dt_outer = bcmVlanActionAdd;
    action.dt_outer_pkt_prio = bcmVlanActionReplace;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id_1, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_translate_action_id_set %d\n", rv);
        return rv;
    }

    /*
    * For mirrored packets, ethernet tag format is  0.
    * This code plays a trick to get the tag_format_class_id when ethernet tag format is 0.
    * tpid1/tpid2 needs to be the TPID 0 in the system.
    */
    bcm_port_tpid_class_t tpid_class;
    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.port = mirror_port;
    tpid_class.tpid1 = 0x8100;
    tpid_class.tpid2 = 0x8100;
    rv = bcm_port_tpid_class_get(unit, &tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_tpid_class_get\n");
        return rv;
    }

    mirror_info.tag_format_class_id = tpid_class.tag_format_class_id;

    /*
    * Set translation action class (map edit_profile & tag_format to action_id) 
    */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = mirror_info.eve_profile;
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    action_class.tag_format_class_id = tpid_class.tag_format_class_id;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2addr, mirror_info.da, mirror_info.vlan);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_l2_addr_add %d\n", rv);
        return rv;
    }

    bcm_mirror_destination_t_init(&dest);
    dest.gport = mirror_port;

    /* create the destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination, return value %d\n", rv);
        return rv;
    }

    mirror_info.mirror_dest_id = dest.mirror_dest_id;

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    /* RSPAN header editing */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = in_port;
    mirror_header_info.tm.out_vport = vlan_port.vlan_port_id;

    if (rv = bcm_mirror_header_info_set(unit, 0, dest.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
        return rv;
    }

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_vlan_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port, in_vlan, G_mirror_type, dest.mirror_dest_id)) {
        printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    if(verbose >= 2 ) {
        printf("created mirror, encap-id: 0x%08x  dest:0x%08x \n", dest.encap_id, dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }

        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }

        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port);
    }

    return rv;
}

/*
 * Clean up procedure for mirror_rspan_example
 */
int mirror_rspan_example_clean_up(int unit) {
    bcm_vlan_translate_action_class_t action_class;
    bcm_pbmp_t p;
    int index;
    int rv;

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_vlan_dest_delete(unit, 
            (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? mirror_info.in_port : mirror_info.out_port, 
            mirror_info.vlan, G_mirror_type, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_vlan_dest_delete %d\n", rv);
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (rv=bcm_l2_addr_delete(unit, mirror_info.da, mirror_info.vlan)) {
        printf("Error: bcm_l2_addr_delete %d\n", rv);
    }

    /*
    * Set translation action class to default
    */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = mirror_info.eve_profile;
    action_class.vlan_translation_action_id = 0;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    action_class.tag_format_class_id = mirror_info.tag_format_class_id;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /*
    * Clear translation action 
    */
    if (rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_EGRESS, mirror_info.eve_action_id)) {
        printf("Error: bcm_vlan_translate_action_id_destroy %d\n", rv);
    }

    if (rv = bcm_vlan_port_destroy(unit, mirror_info.vlan_port_id)) {
        printf("Error: bcm_vlan_port_create %d\n", rv);
        return rv;
    }

    if (!BCM_GPORT_IS_TRUNK(mirror_info.in_port) && !BCM_GPORT_IS_TRUNK(mirror_info.out_port)) {
        BCM_PBMP_PORT_SET(p, mirror_info.in_port);
        BCM_PBMP_PORT_ADD(p, mirror_info.out_port);

        if (rv=bcm_vlan_port_remove(unit, mirror_info.vlan, p)) {
            printf("Error: bcm_vlan_port_remove %d\n", rv);
        }
    } else {
        rv = bcm_vlan_gport_delete(unit, mirror_info.vlan, mirror_info.in_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_delete(vlan=%d)\n", vid);
            return rv;
        }

        rv = bcm_vlan_gport_delete(unit, mirror_info.vlan, mirror_info.out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_delete(vlan=%d)\n", vid);
            return rv;
        }
    }

    if (rv=bcm_vlan_destroy(unit, mirror_info.vlan)) {
        printf("Error: bcm_vlan_destroy %d\n", rv);
    }


    return rv;
}

/*
 * Get incoming tag structure for mirrored packets:
 * - llvp_profile: LLVP profile
 */
uint32 get_mirror_incoming_tag_structure(int unit, uint32 llvp_profile){
    int rv = BCM_E_NONE;
    uint32 entry_handle_id;
    uint32 key_field;
    int inst_id = -1;

    dnx_dbal_entry_handle_take(unit, "EGRESS_LLVP_CLASSIFICATION", &entry_handle_id);
    /*
    * For mirrored packets, IVE ethernet tag format is  0.
    */
    key_field = 0;
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "INNER_TPID", &key_field);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "IS_OUTER_PRIO", &key_field);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "OUTER_TPID", &key_field);
    key_field = 0;
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "EGRESS_LLVP_PROFILE", &key_field);
    dnx_dbal_entry_get(unit, entry_handle_id);
    dnx_dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, "INCOMING_TAG_STRUCTURE", inst_id, &get_val[0]);

    return get_val[0][0];
}

/*
 * RSPAN Advanced mirroring example. Mirrors packets received on in_port (inbound mirroring) or
 * transmitted out from out_port (outbound mirroring) to mirror destination port with VLAN tunnel.
 *
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - rspan_tag : VLAN tag of the RSPAN tunnel
 */
int mirror_rspan_advanced_example(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int rspan_tag) {
    bcm_tunnel_initiator_t rspan_tunnel;
    bcm_tunnel_initiator_t rspan_tunnel_get;
    bcm_l3_intf_t rspan_tunnel_intf;
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_vlan_port_translation_t port_trans;
    bcm_mirror_destination_t dest;
    bcm_vlan_port_t vlan_port;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int name_space = 0x55;
    int action_id_1;
    int index;
    int rv;
 
    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;
    mirror_info.vlan = in_vlan;

    bcm_vlan_create(unit, mirror_info.vlan);    

    if (!BCM_GPORT_IS_TRUNK(in_port) && !BCM_GPORT_IS_TRUNK(out_port)) {
        BCM_PBMP_PORT_SET(p, in_port);
        BCM_PBMP_PORT_ADD(p, out_port);
        BCM_PBMP_CLEAR(u);
        rv = bcm_vlan_port_add(unit, mirror_info.vlan, p, u);
    } else {
        rv = bcm_vlan_gport_add(unit, mirror_info.vlan, in_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add(vlan=%d)\n", vid);
            return rv;
        }

        rv = bcm_vlan_gport_add(unit, mirror_info.vlan, out_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add(vlan=%d)\n", vid);
            return rv;
        }
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT;
    BCM_GPORT_SYSTEM_PORT_ID_SET(vlan_port.port, in_port);
    vlan_port.match_class_id = name_space;
    if (rv = bcm_vlan_port_create(unit, &vlan_port)) {
        printf("Error: bcm_vlan_port_create %d\n", rv);
        return rv;
    }

    mirror_info.vlan_port_id = vlan_port.vlan_port_id;

    /*
    * set edit profile for ingress/egress LIF 
    */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = rspan_tag;
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = mirror_info.eve_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_port_translation_set %d\n", rv);
        return rv;
    }

    /*
    * Create action ID
    */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_translate_action_id_create %d\n", rv);
        return rv;
    }

    mirror_info.eve_action_id = action_id_1;

    /*
    * Set translation action 
    */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = 0x8100;
    action.dt_outer = bcmVlanActionAdd;
    action.dt_outer_pkt_prio = bcmVlanActionReplace;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id_1, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_translate_action_id_set %d\n", rv);
        return rv;
    }

    /*
    * Set translation action class (map edit_profile & tag_format to action_id) 
    */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = mirror_info.eve_profile;
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;


    /*
    * For RSPAN Advanced application, LLVP profile is taken from RSPAN Advanced entry,
    * which is currently not used (set to 0);
    */
    int llvp_profile = 0;
    mirror_info.tag_format_class_id = get_mirror_incoming_tag_structure(unit, llvp_profile);

    action_class.tag_format_class_id = mirror_info.tag_format_class_id;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2addr, mirror_info.da, mirror_info.vlan);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_l2_addr_add %d\n", rv);
        return rv;
    }

    bcm_mirror_destination_t_init(&dest);
    dest.gport = mirror_port;

    /* create the destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination, return value %d\n", rv);
        return rv;
    }

    mirror_info.mirror_dest_id = dest.mirror_dest_id;

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    /* create RSPAN Advanced tunnel */
    bcm_l3_intf_t_init(&rspan_tunnel_intf);
    bcm_tunnel_initiator_t_init(&rspan_tunnel);

    rspan_tunnel.type = bcmTunnelTypeRspanAdvanced;
    rspan_tunnel.class_id = name_space;
 
    if (rv = bcm_tunnel_initiator_create(unit, &rspan_tunnel_intf, &rspan_tunnel)) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    erspan_info.rspan_intf = rspan_tunnel_intf;    

    if (rv = bcm_tunnel_initiator_get(unit, &rspan_tunnel_intf, &rspan_tunnel_get)) {
        printf("Error, get RSPAN Advanced tunnel initiator \n");
        return rv;
    }

    if ((rspan_tunnel_get.type != bcmTunnelTypeRspanAdvanced) || (rspan_tunnel_get.class_id != rspan_tunnel.class_id)) {
        printf("Error, RSPAN Advanced tunnel initiator get mismatch\n");
        print rspan_tunnel;
        print rspan_tunnel_get;
        return BCM_E_FAIL;
    }

    /* ERSPAN header editing */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = in_port;
    mirror_header_info.tm.out_vport = rspan_tunnel.tunnel_id;

    if (rv = bcm_mirror_header_info_set(unit, 0, dest.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
        return rv;
    }

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_vlan_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port, in_vlan, G_mirror_type, dest.mirror_dest_id)) {
        printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    if(verbose >= 2 ) {
        printf("created mirror, encap-id: 0x%08x  dest:0x%08x \n", dest.encap_id, dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }

        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }

        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port);
    }

    return rv;
}

/*
 * Clean up procedure for mirror_rspan_advanced_example
 */
int mirror_rspan_advanced_example_clean_up(int unit) {
    bcm_vlan_translate_action_class_t action_class;
    bcm_pbmp_t p;
    int index;
    int rv;

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_vlan_dest_delete(unit, 
            (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? mirror_info.in_port : mirror_info.out_port, 
            mirror_info.vlan, G_mirror_type, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_vlan_dest_delete %d\n", rv);
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (rv = bcm_tunnel_initiator_clear(unit, &erspan_info.rspan_intf)) {
        printf("Error: bcm_tunnel_initiator_clear %d\n", rv);
    }

    if (rv=bcm_l2_addr_delete(unit, mirror_info.da, mirror_info.vlan)) {
        printf("Error: bcm_l2_addr_delete %d\n", rv);
    }

    /*
    * Set translation action class to default
    */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = mirror_info.eve_profile;
    action_class.vlan_translation_action_id = 0;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    action_class.tag_format_class_id = mirror_info.tag_format_class_id;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /*
    * Clear translation action 
    */
    if (rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_EGRESS, mirror_info.eve_action_id)) {
        printf("Error: bcm_vlan_translate_action_id_destroy %d\n", rv);
    }

    if (rv = bcm_vlan_port_destroy(unit, mirror_info.vlan_port_id)) {
        printf("Error: bcm_vlan_port_create %d\n", rv);
        return rv;
    }

    if (!BCM_GPORT_IS_TRUNK(mirror_info.in_port) && !BCM_GPORT_IS_TRUNK(mirror_info.out_port)) {
        BCM_PBMP_PORT_SET(p, mirror_info.in_port);
        BCM_PBMP_PORT_ADD(p, mirror_info.out_port);

        if (rv=bcm_vlan_port_remove(unit, mirror_info.vlan, p)) {
            printf("Error: bcm_vlan_port_remove %d\n", rv);
        }
    } else {
        rv = bcm_vlan_gport_delete(unit, mirror_info.vlan, mirror_info.in_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_delete(vlan=%d)\n", vid);
            return rv;
        }

        rv = bcm_vlan_gport_delete(unit, mirror_info.vlan, mirror_info.out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_delete(vlan=%d)\n", vid);
            return rv;
        }
    }

    if (rv=bcm_vlan_destroy(unit, mirror_info.vlan)) {
        printf("Error: bcm_vlan_destroy %d\n", rv);
    }


    return rv;
}


int mirror_with_span_lag_lb_example(int unit, int in_port, int in_vlan, int dest_port, int trunk_id) {
    int rv;
    bcm_gport_t mirror_port;
    BCM_GPORT_TRUNK_SET(mirror_port,trunk_id);
    return mirror_with_span_example(unit,in_port,in_vlan,dest_port,mirror_port);
}

 /*
  * Change default configuration for ERSPAN tunnel.
  */
 int mirror_erspan_tunnel_init(
     int is_erspan_v3,
     int vlan,
     int cos,
     int truncated,
     int direction,
     int session_id,
     int en,
     int index_17_19,
     int hw_id,
     int switch_id,
     bcm_ip_t dip,
     bcm_ip_t sip,
     int dscp,
     int ttl,
     bcm_mac_t next_hop,
     bcm_vlan_t vid)
 {
     erspan_info.is_erspan_v3 = is_erspan_v3;
     erspan_info.vlan = vlan;
     erspan_info.cos = cos;
     erspan_info.truncated = truncated;
     erspan_info.direction = direction;
     erspan_info.session_id = session_id;
     erspan_info.en = en;
     erspan_info.index_17_19 = index_17_19;
     erspan_info.hw_id = hw_id;
     erspan_info.switch_id = switch_id;
     erspan_info.dip = dip;
     erspan_info.sip = sip;
     erspan_info.dscp = dscp;
     erspan_info.ttl = ttl;
     erspan_info.next_hop = next_hop;
     erspan_info.vid = vid;
 
     return 0;
 }
 
  /*
   * Create basic routing application, RIF, MyMAC...etc.
   * - in_port : port that packets to be recieved from
   * - out_port : port that packets to be transmitted out
   */
 int
 erspan_routing(
     int unit,
     int in_port,
     int out_port)
 {
     int kaps_result;

     get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &kaps_result);
     return dnx_basic_example(unit, in_port, out_port, kaps_result);
}

/*
 * Init counter engine for ERSPAN Sequence Number.
 * - counter_command_id : Counter interface to get the Sequence Number (0~2)
 * - counter_engine_id : Counter engine to be associated with counter interface
 * - database_id : counter database ID
 */
int
erspan_init_counter_engine(
    int unit,
    int counter_command_id,
    int counter_engine_id,
    int database_id)
{
    uint32 actual_nof_counters;
    uint32 nof_cores;
    uint32 core_id;
    int engine_id;
    int rv = BCM_E_NONE;

    erspan_info.counter_command_id = counter_command_id;
    erspan_info.counter_engine_id = counter_engine_id;
    erspan_info.database_id = database_id;

    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));

    for (core_id=0; core_id<nof_cores; ++core_id)
    {
        engine_id = counter_engine_id + core_id;

        rv = crps_oam_database_set(unit, core_id, bcmStatCounterInterfaceEgressOam, counter_command_id, 1, &engine_id, 0, database_id+core_id, &actual_nof_counters);
        if (rv != BCM_E_NONE)
        {
            printf("Error: crps_oam_database_set core_id %d\n", core_id);
            return rv;
        }
    }

    return rv;
}


/*
 * Create ERSPANv2/v3 tunnel and return the encap_id.
 * - encap_id : point to the created ERSPAN tunnel
 */
int mirror_erspan_tunnel_create(int unit, bcm_if_t *encap_id) {
    bcm_tunnel_initiator_t ip_tunnel;
    bcm_tunnel_initiator_t erspan_tunnel;
    bcm_tunnel_initiator_t erspan_tunnel_get;
    bcm_l3_intf_t ip_tunnel_intf;
    bcm_l3_intf_t erspan_tunnel_intf;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null = 0;
    bcm_gport_t arp_ac_gport;
    int vlan_edit_profile = 7;
    int rv;

    /* Next hop */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = erspan_info.next_hop;
    l3eg.flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    l3eg.vlan = erspan_info.vid;

    if (rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null)) {
        printf("Error: bcm_l3_egress_create %d\n", rv);
        return rv;
    }

    erspan_info.next_hop_if = l3eg.encap_id;

    if (verbose >= 1) {
        printf("next hop mac at encap-id 0x%08x, \n", l3eg.encap_id);
    }

    /* create IPv4 tunnel for ERSPAN */
    bcm_tunnel_initiator_t_init(&ip_tunnel);
    if (G_mirror_tunnel_is_ipv6)
    {
        ip_tunnel.dip6 = erspan_info.dip6;
        ip_tunnel.sip6 = erspan_info.sip6;
        ip_tunnel.type = bcmTunnelTypeGreAnyIn6;
    }
    else
    {
    ip_tunnel.dip = erspan_info.dip;
    ip_tunnel.sip = erspan_info.sip;
        ip_tunnel.type = bcmTunnelTypeGreAnyIn4;
    }
    ip_tunnel.dscp = erspan_info.dscp;
    ip_tunnel.ttl = erspan_info.ttl;
	ip_tunnel.l3_intf_id = l3eg.encap_id;
    ip_tunnel.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    ip_tunnel.encap_access = bcmEncapAccessTunnel2;
    ip_tunnel.flags = BCM_TUNNEL_INIT_GRE_WITH_SN;

    bcm_l3_intf_t_init(&ip_tunnel_intf);
    if (rv = bcm_tunnel_initiator_create(unit, &ip_tunnel_intf, &ip_tunnel)) {
        printf("Error, bcm_tunnel_initiator_create %d\n", rv);
        return rv;
    }

    erspan_info.ip_tunnel_intf = ip_tunnel_intf;

    if (verbose >= 1) {
        printf("created IP tunnel on intf: 0x%08x \n", ip_tunnel_intf.l3a_intf_id);
    }

    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_ac_gport, BCM_L3_ITF_VAL_GET(l3eg.encap_id));
    BCM_GPORT_VLAN_PORT_ID_SET(arp_ac_gport, arp_ac_gport);

    /*
    * Perform vlan editing for the ARP gport.
    * This will add erspan_info.vid as the VLAN tag
    */
    rv = vlan_translate_ive_eve_translation_set(unit, arp_ac_gport,         /* lif  */
                                                      0x8100,               /* outer_tpid */
                                                      0x8100,               /* inner_tpid */
                                                      bcmVlanActionMappedReplace,  /* outer_action */
                                                      bcmVlanActionNone,                 /* inner_action */
                                                      0,                 /* new_outer_vid */
                                                      0,               /* new_inner_vid */
                                                      vlan_edit_profile,    /* vlan_edit_profile */
                                                      0,                    /* tag_format - must be untag! */
                                                      FALSE                 /* is_ive */
                                                      );

    if (rv != BCM_E_NONE) {
        printf("Error: vlan_translate_ive_eve_translation_set %d\n", rv);
        return rv;
    }

    /* create ERSPAN tunnel */
    bcm_l3_intf_t_init(&erspan_tunnel_intf);
    bcm_tunnel_initiator_t_init(&erspan_tunnel);

    erspan_tunnel.type = bcmTunnelTypeErspan;
    erspan_tunnel.l3_intf_id = ip_tunnel_intf.l3a_intf_id;
    erspan_tunnel.encap_access = bcmEncapAccessTunnel1;

    if (erspan_info.gre_with_sn)
    {
        erspan_tunnel.flags = BCM_TUNNEL_INIT_ERSPAN_WITH_SN;
        erspan_tunnel.stat_cmd = 10;
        erspan_tunnel.counter_command_id = erspan_info.counter_command_id;
    }

    if (erspan_info.is_erspan_v3) {
        erspan_tunnel.flags |= BCM_TUNNEL_INIT_ERSPAN_TYPE3;
        erspan_tunnel.hw_id = erspan_info.hw_id;
        erspan_tunnel.switch_id = erspan_info.switch_id;
    } else {
        if (mirror_is_jr2_system_header_mode(unit))
        {
            erspan_tunnel.aux_data = erspan_info.index_17_19;
        }
        else
        {
            erspan_tunnel.span_id = erspan_info.session_id;
        }
    }

    if (rv = bcm_tunnel_initiator_create(unit, &erspan_tunnel_intf, &erspan_tunnel)) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    erspan_info.erspan_intf = erspan_tunnel_intf;

    if (verbose >= 1) {
        printf("created ERSPAN tunnel on intf: 0x%08x \n", erspan_tunnel.tunnel_id);
    }

    if (rv = bcm_tunnel_initiator_get(unit, &erspan_tunnel_intf, &erspan_tunnel_get)) {
        printf("Error, get ERSPAN tunnel initiator \n");
        return rv;
    }

    if ((erspan_info.is_erspan_v3 
            && (((erspan_tunnel_get.flags & BCM_TUNNEL_INIT_ERSPAN_TYPE3) != BCM_TUNNEL_INIT_ERSPAN_TYPE3)
                || (erspan_tunnel_get.type != bcmTunnelTypeErspan)
                || (erspan_tunnel_get.hw_id != erspan_tunnel.hw_id)
                || (erspan_tunnel_get.switch_id != erspan_tunnel.switch_id)))
        ||
        (!erspan_info.is_erspan_v3 
            && (((erspan_tunnel_get.flags & BCM_TUNNEL_INIT_ERSPAN_TYPE3) != 0)
                || (erspan_tunnel_get.aux_data != erspan_tunnel.aux_data)))) {
        printf("Error, ERSPAN tunnel initiator get mismatch\n");
        print erspan_tunnel;
        print erspan_tunnel_get;
        return BCM_E_FAIL;
    }

    *encap_id = erspan_tunnel.tunnel_id;

    return rv;
}

/*
 * Configure pipeline to mirror the packet to ERSPAN tunnel.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - is_mc : set if mirror destination is a multicast group
 * - mc_id : multicast ID in case mirror destination is a multicast group
 * - encap_id : point to the ERSPAN tunnel
 */
int mirror_erspan_destination_create(int unit, int in_port, bcm_vlan_t in_vlan, int out_port, int mirror_port, int is_mc, int mc_id, bcm_if_t encap_id) {
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t mirror_header_info_get;
    bcm_mirror_destination_t dest;
    uint32 flags = 0;
    int rv;

    bcm_mirror_destination_t_init(&dest);
    if (is_mc == TRUE) {
        /* create multicat group and replicate to ERSPAN tunnel */
        bcm_multicast_replication_t replications;
        if (rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP, mc_id)) {
            printf("Error: bcm_multicast_create %d\n", rv);
            return rv;
        }

        bcm_multicast_replication_t_init(&replications);
        replications.encap1 = BCM_GPORT_TUNNEL_ID_GET(encap_id);
        replications.port = mirror_port;

        if (rv = bcm_multicast_add(unit, mc_id, BCM_MULTICAST_EGRESS_GROUP, 1, &replications)) {
            printf("Error, create ergress multicast\n");
            return rv;
        }

        if (rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_id, mc_id))
        {
            printf("Error: multicast__open_fabric_mc_or_ingress_mc_for_egress_mc %d\n", rv);
            return rv;
        }

        if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)) {
            BCM_GPORT_MCAST_SET(dest.gport, mc_id);
        }
        else
        {
            int erp_port;
            int count_erp;
            int sysport;

            /*
            * On Q2A for egress MC, TM destination at ingress should be ERP port + MC ID.
            * MC ID will be configured in bcm_mirror_header_info_set.
            */
            rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, 1, &erp_port, &count_erp);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_internal_get \n");
                return rv;
            }

            rv = bcm_stk_gport_sysport_get(unit, erp_port, &dest.gport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_stk_gport_sysport_get \n");
                return rv;
            }
        }
    }
    else {
        dest.gport = mirror_port;
    }

    /* create mirror destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination, return value %d\n", rv);
        return rv;
    }

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    mirror_info.mirror_dest_id = dest.mirror_dest_id;

    /* ERSPAN header editing */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = in_port;
    mirror_header_info.tm.ase_ext.valid = TRUE;
    mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeErspan;
    mirror_header_info.tm.ase_ext.ase_info.erspan_info.vlan = erspan_info.vlan;
    mirror_header_info.tm.ase_ext.ase_info.erspan_info.cos = erspan_info.cos;
    mirror_header_info.tm.ase_ext.ase_info.erspan_info.direction = (G_mirror_type == BCM_MIRROR_PORT_INGRESS)?0:1;

    if (is_mc == TRUE) {
        mirror_header_info.tm.is_mc_traffic = TRUE;
        BCM_GPORT_MCAST_SET(mirror_header_info.tm.mc_id, mc_id);
    } else {
        mirror_header_info.tm.out_vport = encap_id;
    }

    if (rv = bcm_mirror_header_info_set(unit, 0, dest.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
        return rv;
    }

    bcm_mirror_header_info_t_init(&mirror_header_info_get);
    if (rv = bcm_mirror_header_info_get(unit, dest.mirror_dest_id, &flags, &mirror_header_info_get)) {
        printf("Failed bcm_mirror_header_info_get, return value %d\n", rv);
        return rv;
    }

    /* JR1 does not have ASE header */
    if (mirror_is_jr2_system_header_mode(unit) &&
        ((mirror_header_info.tm.ase_ext.valid != mirror_header_info_get.tm.ase_ext.valid) ||
        (mirror_header_info.tm.ase_ext.ase_type != mirror_header_info_get.tm.ase_ext.ase_type) ||
        (mirror_header_info.tm.ase_ext.ase_info.erspan_info.vlan != mirror_header_info_get.tm.ase_ext.ase_info.erspan_info.vlan))) {
        printf("Failed to check erspan information.\n");
        return BCM_E_FAIL;
    }

    /* set <port,vlan> mirroring */
    if (G_mirror_type == BCM_MIRROR_PORT_INGRESS) {
        if (rv = bcm_mirror_port_vlan_dest_add(unit, in_port, mirror_info.vlan, G_mirror_type, dest.mirror_dest_id)) {
            printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
            return rv;
        }
    }
    else {
        /* out_port + in_vlan, for bridged packets */
        if (rv = bcm_mirror_port_vlan_dest_add(unit, out_port, mirror_info.vlan, G_mirror_type, dest.mirror_dest_id)) {
            printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
            return rv;
        }

        /* out_port + out_vlan, for routed packets */
        if (rv = bcm_mirror_port_vlan_dest_add(unit, out_port, mirror_info.out_vlan, G_mirror_type, dest.mirror_dest_id)) {
            printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
            return rv;
        }
    }

    if(verbose >= 2 ) {
        printf("created mirror, encap-id: 0x%08x  dest:0x%08x \n", encap_id, dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }

        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }

        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port);
    }

    return rv;
}

/*
 * Example of mirroring packet to a ERSPANv2/v3 tunnel.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 */
int mirror_erspan_example(int unit, int in_port, bcm_vlan_t in_vlan, int out_port, int mirror_port){
    bcm_if_t encap_id;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int rv;

    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;
    mirror_info.vlan = in_vlan;

    bcm_vlan_create(unit, in_vlan);
    erspan_routing(unit, in_port, out_port);


    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, out_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, in_vlan, p, u);

    mirror_info.vlan = in_vlan;

    l2addr.mac = mirror_info.da;
    l2addr.vid = mirror_info.vlan;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);

    if (erspan_info.gre_with_sn)
    {
        if (rv = erspan_init_counter_engine(unit, 0, 0, 0))
        {
            printf("Error, erspan_init_counter_engine \n");
            return rv;
        }
    }

    /* build IP tunnels, and get back interfaces */
    if (rv = mirror_erspan_tunnel_create(unit, &encap_id)) {
        printf("Error, mirror_erspan_tunnel_create \n");
        return rv;
    }

    /* add inbound mirror to send out of the create ip tunnel with span encapsulation */
    rv = mirror_erspan_destination_create(unit, in_port, in_vlan, out_port, mirror_port, 0, 0, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in mirror_erspan_destination_create, in_port=%d, \n", in_port);
        return rv;
    }

    return rv;
}

/*
 * Example of mirroring packet to a ERSPANv2 tunnel.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 */
int mirror_erspan_example_v2(int unit, int in_port, bcm_vlan_t in_vlan, int out_port, int mirror_port){
    int rv;

    erspan_info.is_erspan_v3 = FALSE;

    rv = mirror_erspan_example(unit, in_port, in_vlan, out_port, mirror_port);

    return rv;
}

/*
 * Example of mirroring packet to a ERSPANv3 tunnel.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 */
int mirror_erspan_example_v3(int unit, int in_port, int in_vlan, int out_port, int mirror_port){
    int rv;

    erspan_info.is_erspan_v3 = TRUE;

    rv = mirror_erspan_example(unit, in_port, in_vlan, out_port, mirror_port);

    return rv;
}

/*
 * Example of mirroring packet to a ERSPAN tunnel using egress multicast.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - mc_id : multicast ID of the mirror destination
 */
int mirror_erspan_multicast_example(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int mc_id){
    bcm_l2_addr_t l2addr;
    bcm_if_t encap_id;
    bcm_pbmp_t p, u;
    int rv;

    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;
    mirror_info.vlan = in_vlan;
    mirror_info.is_mc = 1;
    mirror_info.mc_id = mc_id;
    bcm_vlan_create(unit, in_vlan);
    erspan_routing(unit, in_port, out_port);

    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, out_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, in_vlan, p, u);

    l2addr.mac = mirror_info.da;
    l2addr.vid = mirror_info.vlan;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);

    /* build IP tunnels, and get back interfaces */
    if (rv = mirror_erspan_tunnel_create(unit, &encap_id)) {
        printf("Error, mirror_erspan_tunnel_create \n");
        return rv;
    }

    /* add inbound mirror to send out of the create ip tunnel with span encapsulation */
    rv = mirror_erspan_destination_create(unit, in_port, in_vlan, out_port, mirror_port, 1, mc_id, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in mirror_erspan_destination_create, in_port=%d, \n", in_port);
        return rv;
    }

    return rv;
}

/*
 * Clean up procedure for mirror_erspan_example
 */
int mirror_erspan_example_clean_up(int unit){
    bcm_mirror_header_info_t mirror_header_info;
    bcm_pbmp_t p;
    uint32 nof_cores;
    uint32 core_id;
    int rv;

    if (erspan_info.gre_with_sn)
    {
        nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));

        for (core_id=0; core_id<nof_cores; ++core_id)
        {
            crps_oam_database_destroy(unit, core_id, erspan_info.database_id+core_id);
        }
    }

    if (rv = bcm_mirror_port_vlan_dest_delete(unit, 
            (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? mirror_info.in_port : mirror_info.out_port, 
            mirror_info.vlan, G_mirror_type, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
        /* out_port + out_vlan, for routed packets */
        if (rv = bcm_mirror_port_vlan_dest_delete(unit, mirror_info.out_port, mirror_info.out_vlan, G_mirror_type, mirror_info.mirror_dest_id)) {
            printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
        }
    }

    bcm_mirror_header_info_t_init(&mirror_header_info);
    if (rv = bcm_mirror_header_info_set(unit, 0, mirror_info.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (mirror_info.is_mc) {
        if (rv=bcm_multicast_destroy(unit, mirror_info.mc_id)) {
            printf("Error: bcm_multicast_destroy %d\n", rv);
        }
    }

    if (rv = bcm_tunnel_initiator_clear(unit, &erspan_info.erspan_intf)) {
        printf("Error: bcm_tunnel_initiator_clear %d\n", rv);
    }

    if (rv = bcm_tunnel_initiator_clear(unit, &erspan_info.ip_tunnel_intf)) {
        printf("Error: bcm_tunnel_initiator_clear %d\n", rv);
    }

    if (rv = bcm_l3_egress_destroy(unit, erspan_info.next_hop_if)) {
        printf("Error: bcm_l3_egress_destroy %d\n", rv);
    }

    if (rv=bcm_l2_addr_delete(unit, mirror_info.da, mirror_info.vlan)) {
        printf("Error: bcm_l2_addr_delete %d\n", rv);
    }

    BCM_PBMP_PORT_SET(p, mirror_info.in_port);
    BCM_PBMP_PORT_ADD(p, mirror_info.out_port);

    if (rv=bcm_vlan_port_remove(unit, mirror_info.vlan, p)) {
        printf("Error: bcm_vlan_port_remove %d\n", rv);
    }

    return BCM_E_NONE;
}

/**
  * Set outbound mirroring, carry system header and egress TOD to 2nd cycle.
  *
  * - in_port : port that packets to be recieved from
  * - out_port : port that packets to be forwarded out
  * - mirror_port : mirror destination port
  */
int mirror_egress_probabiliy_sample_mirror_config(int unit, int in_port, int out_port, int mirror_port) {
    bcm_mirror_destination_t dest;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int rv;

    bcm_vlan_create(unit, mirror_prob_sample_info.vlan);

    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, out_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, mirror_prob_sample_info.vlan, p, u);

    l2addr.mac = mirror_prob_sample_info.da;
    l2addr.vid = mirror_prob_sample_info.vlan;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);

    /**
     * Need to set the BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER flag
     * in order to carry egress TOD to the 2nd cycle for the mirrored packets.
     */
    bcm_mirror_destination_t_init(&dest);
    dest.gport = mirror_port;
    dest.flags |= BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER;

    /** create the destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("Error: bcm_mirror_destination_create %d\n", rv);
        return rv;
    }

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    /** set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_dest_add(unit, out_port, BCM_MIRROR_PORT_EGRESS, dest.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_add %d\n", rv);
        return rv;
    }

    mirror_prob_sample_info.mirror_dest_id = dest.mirror_dest_id;

    return rv;
}

/**
  * Create mirror RCY context, set compare mode to compare egress TOD to PP port key GEN var
  *
  * - compare_bits : number of bits to be taken from egress TOD to compare with PP port key GEN var
  */
int mirror_egress_probabiliy_sample_compare_config(int unit, uint32 compare_bits)
{
    bcm_field_qualifier_info_create_t pp_port_key_gen_qual_info;
    bcm_field_qualifier_info_get_t pp_port_key_gen_info_get;
    bcm_field_qualify_t pp_port_key_gen_qual_id;
    bcm_field_qualifier_info_create_t egress_tod_qual_info;
    bcm_field_qualify_t egress_tod_qual_id;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_context_info_t context_info;
    bcm_field_context_t context_id;
    void *dest_char;
    int rv;

    /**
     * Create context for mirror probability sampling PMF group.
     */
    bcm_field_context_info_t_init(&context_info);

    /**
     * Using firlst compare group (keys F & G), and compare key F MSB to key G MSB.
     */
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "egr_mir_prob_sample", sizeof(context_info.name));

    if (rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id))
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /** Create UDF to stamp PP port key GEN var for mirror recycle port */
    bcm_field_qualifier_info_create_t_init(&pp_port_key_gen_qual_info);
    pp_port_key_gen_qual_info.size = compare_bits;
    dest_char = &(pp_port_key_gen_qual_info.name[0]);
    sal_strncpy_s(dest_char, "PP_port_key_GEN", sizeof(pp_port_key_gen_qual_info.name));

    if (rv = bcm_field_qualifier_create(unit, 0, &pp_port_key_gen_qual_info, &pp_port_key_gen_qual_id))
    {
        printf("Error (%d), in bcm_field_qualifier_create when creating PP port key gen UDF\n", rv);
        return rv;
    }

    /** Create UDF to stamp egress TOD from first cycle */
    bcm_field_qualifier_info_create_t_init(&egress_tod_qual_info);
    egress_tod_qual_info.size = compare_bits;
    dest_char = &(egress_tod_qual_info.name[0]);
    sal_strncpy_s(dest_char, "egress_TOD", sizeof(egress_tod_qual_info.name));

    if (rv = bcm_field_qualifier_create(unit, 0, &egress_tod_qual_info, &egress_tod_qual_id))
    {
        printf("Error (%d), in bcm_field_qualifier_create when creating egress TOD UDF\n", rv);
        return rv;
    }

    if (rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyPortClassPacketProcessingGeneralData, bcmFieldStageIngressPMF1, &pp_port_key_gen_info_get))
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }

    /**
     * Key F MSB = PP port key GEN var[n:0]
     * Key G MSB = egress TOD[n:0]
     */
    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = pp_port_key_gen_qual_id;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    compare_info.first_key_info.qual_info[0].input_arg = 0;
    compare_info.first_key_info.qual_info[0].offset = pp_port_key_gen_info_get.offset;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = egress_tod_qual_id;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.second_key_info.qual_info[0].input_arg = 0;
    compare_info.second_key_info.qual_info[0].offset = 64*8-compare_bits;

    /**
     * Create compare context using the first compare key groups
     */
    if (rv = bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 0, &compare_info))
    {
        printf("Error (%d), in bcm_field_context_compare_create\n", rv);
        return rv;
    }

    mirror_prob_sample_info.pp_port_key_gen_qual_id = pp_port_key_gen_qual_id;
    mirror_prob_sample_info.egress_tod_qual_id = egress_tod_qual_id;
    mirror_prob_sample_info.ipmf1_context_id = context_id;

    return rv;
}

/**
  * Set preselector for mirror RCY context to select mirror RCY port using PP port CS profile
  */
int mirror_egress_probabiliy_sample_presel_set(int unit)
{
    bcm_field_presel_entry_data_t presel_info;
    bcm_field_presel_entry_id_t presel_ent;
    int rv;

    /**
     * Select mirror RCY port using PP port CS profile.
     * Additional mirror RCY ports can be selected using the same CS profile.
     */
    presel_ent.presel_id = mirror_prob_sample_info.ipmf1_presel_id;
    presel_ent.stage = bcmFieldStageIngressPMF1;
    presel_info.context_id = mirror_prob_sample_info.ipmf1_context_id;
    presel_info.entry_valid = TRUE;
    presel_info.nof_qualifiers = 1;
    presel_info.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_info.qual_data[0].qual_value = mirror_prob_sample_info.rcy_cs_profile;
    presel_info.qual_data[0].qual_mask = 0x7;

    if (rv = bcm_field_presel_set(unit, 0, &presel_ent, &presel_info))
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }

    return rv;
}

/**
  * Config probability sampling field group to clear mirror profile when egress TOD > PP port key GEN var
  */
int mirror_egress_probabiliy_sample_fg_config(int unit)
{
    bcm_field_qualifier_info_create_t mirror_code_qual_info;
    bcm_field_qualify_t mirror_code_qual_id;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id;
    bcm_field_entry_t ent;
    bcm_gport_t null_mirror_dest;
    void *dest_char;
    int op_offset;
    int rv;

    /** Create UDF to stamp mirror code */
    bcm_field_qualifier_info_create_t_init(&mirror_code_qual_info);
    mirror_code_qual_info.size = 4;
    dest_char = &(mirror_code_qual_info.name[0]);
    sal_strncpy_s(dest_char, "mirror_code_udf", sizeof(mirror_code_qual_info.name));

    if (rv = bcm_field_qualifier_create(unit, 0, &mirror_code_qual_info, &mirror_code_qual_id))
    {
        printf("Error (%d), in bcm_field_qualifier_create when creating mirror code UDF\n", rv);
        return rv;
    }

    /**
     * Create first PMF group in iPMF2, that when (egress_TOD >= key GEN var) && (mirror_profile == mirror_profile_created),
     * set mirror code to 0 (clear mirror action).
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    /**
     * Selects compare result and mirror code
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyCompareKeysResult0;
    fg_info.qual_types[1] = mirror_code_qual_id;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionMirrorIngress;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "mir_prob_smpl_ipmf2", sizeof(fg_info.name));

    if (rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id))
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /**
     * Get the operand offset for key F MSB <= key G MSB (key GEN var<=egress TOD)
     */
    if (rv = bcm_field_compare_operand_offset_get(unit, 1, bcmFieldCompareOperandFirstKeyNotBigger, &op_offset))
    {
        printf("Error (%d), in bcm_field_compare_operand_offset_get\n", rv);
        return rv;
    }

    /**
     * Attach information:
     *     key = {compare result 0, mirror_code[3:0]}
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = 2;
    attach_info.key_info.qual_types[0] = bcmFieldQualifyCompareKeysResult0;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[1] = mirror_code_qual_id;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    
    attach_info.key_info.qual_info[1].offset = mirror_prob_sample_info.mirror_code_ipmf1_offset;
    attach_info.payload_info.nof_actions = 1;
    attach_info.payload_info.action_types[0] = bcmFieldActionMirrorIngress;

    if (rv = bcm_field_group_context_attach(unit, 0, fg_id, mirror_prob_sample_info.ipmf1_context_id, &attach_info))
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /**
     * if egress TOD > key GEN var and mirror code match, set mirror code to 0 (clear mirror action).
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 2;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyCompareKeysResult0;
    entry_info.entry_qual[0].value[0] = 1 << op_offset;
    entry_info.entry_qual[0].mask[0] = 1 << op_offset;
    entry_info.entry_qual[1].type = mirror_code_qual_id;
    entry_info.entry_qual[1].value[0] = BCM_GPORT_MIRROR_GET(mirror_prob_sample_info.mirror_dest_id);
    entry_info.entry_qual[1].mask[0] = 0xF;
    entry_info.entry_action[0].type = bcmFieldActionMirrorIngress;

    BCM_GPORT_MIRROR_MIRROR_SET(null_mirror_dest, 0);
    entry_info.entry_action[0].value[0] = null_mirror_dest;

    if (rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &ent))
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    mirror_prob_sample_info.mirror_code_qual_id = mirror_code_qual_id;
    mirror_prob_sample_info.prob_sample_fg_id = fg_id;
    mirror_prob_sample_info.prob_sample_ent = ent;

    return rv;
}

/**
  * Config strip header field group to strip 64B from packet start
  */
int mirror_egress_probabiliy_sample_strip_header_config(int unit)
{
    bcm_field_presel_entry_data_t presel_info;
    bcm_field_presel_entry_id_t presel_ent;
    bcm_field_context_info_t context_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t ent;
    bcm_gport_t null_mirror_dest;
    bcm_field_context_t context_id;
    void *dest_char;
    int rv = BCM_E_NONE;


    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "egr_mir_strip_hdr", sizeof(context_info.name));

    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    presel_ent.presel_id = mirror_prob_sample_info.ipmf1_presel_id;
    presel_ent.stage = bcmFieldStageIngressPMF3;

    presel_info.context_id = context_id;
    presel_info.entry_valid = TRUE;
    presel_info.nof_qualifiers = 1;
    presel_info.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_info.qual_data[0].qual_value = mirror_prob_sample_info.rcy_cs_profile;
    presel_info.qual_data[0].qual_mask = 0x7;

    rv = bcm_field_presel_set(unit, 0, &presel_ent, &presel_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = mirror_prob_sample_info.mirror_code_qual_id;
    fg_info.action_types[0] = bcmFieldActionStartPacketStrip;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = 1;
    attach_info.payload_info.nof_actions = 1;
    attach_info.key_info.qual_types[0] = mirror_prob_sample_info.mirror_code_qual_id;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = mirror_prob_sample_info.mirror_code_ipmf3_offset;
    attach_info.payload_info.action_types[0] =  bcmFieldActionStartPacketStrip;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /**
     * If mirror code match, strip 64 bytes from packet start
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = mirror_prob_sample_info.mirror_code_qual_id;
    entry_info.entry_qual[0].value[0] = BCM_GPORT_MIRROR_GET(mirror_prob_sample_info.mirror_dest_id);
    entry_info.entry_qual[0].mask[0] = 0xF;
    entry_info.entry_action[0].type = bcmFieldActionStartPacketStrip;
    entry_info.entry_action[0].value[0] = bcmFieldStartToConfigurableStrip;
    entry_info.entry_action[0].value[1] = 64;

    if (rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &ent))
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    mirror_prob_sample_info.ipmf3_context_id = context_id;
    mirror_prob_sample_info.strip_header_fg_id = fg_id;
    mirror_prob_sample_info.strip_header_ent = ent;

    return rv;
}

/*
 * PMF example for egress mirror probability sampling.
 * - in_port : port that packets to be recieved from
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - prop : mirror probability. Actual sample rate = prop/(1<<nof_bits)
 * - compare_bits : number of bits to compare between egress TOD and PP port key GEN value. Prefer to use smaller values here.
 *
 * Configuration sequence:
 * 1. Set outbound mirroring with BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER flag
 *     This will prepend a 64B system header, as well as the mirror additional information, to the mirrored copy of the packet.
 *     The mirror packet received at the mirror recycle port will have the following format:
 *         {[System_Header, ingress TOD, egress TOD](total 64B), payload}
 * 2. Set PP port key GEN var of the mirror recycle port to 'prop'
 *     mirror recycle ports are the ports defined in config.bcm as follows:
 *         ucode_port_100.BCM8869X=RCY_MIRROR.0:core_0.100
 * 3. Create mirror recycle probability sampling context, select mirror recycle port(s) using PP port PMF CS profile
 * 4. Set context to compare egress TOD in packet header to PP port key GEN var
 * 5. Create PMF group and qualify compare result, if egress TOD > prop, then clear mirror action
 * 6. To archieve 100% sample probability, set PP port PMF CS profile to the default value so that it will not be selected by this progam.
 *
 * run:
 *     cint sand/utility/cint_sand_utils_global.c
 *     cint dnx/cint_dnx_mirror_example.c
 *         cint
 *         mirror_egress_probabiliy_sample_example(0, 200, 13, 16, 0x80, 8);
 *         exit;
 *
 *         clear coun
 *         tx 200 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *         show coun
 *
 *         expect "snmpEtherStatsTXNoErrors" on port 16 is ~100 (50%, can be in the range of 70~130)
 */
int mirror_egress_probabiliy_sample_example(int unit, int in_port, int out_port, int mirror_port, uint32 prob, uint32 compare_bits) {
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;
    bcm_gport_t rcy_mirror_gport;
    int rv = BCM_E_NONE;

    if (*(dnxc_data_get(unit, "snif", "egress", "probability_sample", NULL)))
    {
        /**
         * If hardware supports probability sampling, then configure it directly.
         * Otherwise configure PMF.
         */
        set_outbound_mode(unit, 1);
        mirror_info.egress_sample_rate_dividend = prob;
        mirror_info.egress_sample_rate_divisor = 0x1<<compare_bits;
        return mirror_span_example(unit, in_port, out_port, mirror_port);
    }

    mirror_prob_sample_info.in_port = in_port;
    mirror_prob_sample_info.out_port = out_port;
    mirror_prob_sample_info.mirror_port = mirror_port;

    /**
     * Configure outbound mirroring with BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER.
     * This will carry a 64B system header, as well as egress TOD to the 2nd pass.
     */
    rv = mirror_egress_probabiliy_sample_mirror_config(unit, in_port, out_port, mirror_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mirror_egress_probabiliy_sample_mirror_config\n", rv);
        return rv;
    }

    /**
     * Create mirror RCY context, compare egress TOD with PP port key GEN var
     */
    rv = mirror_egress_probabiliy_sample_compare_config(unit, compare_bits);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mirror_egress_probabiliy_sample_compare_config\n", rv);
        return rv;
    }

    /**
      * Get mirror recycle port for outbound mirroring.
      * For each out TM port there is a corresponding mirror recycle port defined for it.
      * This is the RX port resolved for outbound mirrored packets after recycling.
      */
    if (rv=bcm_mirror_port_to_rcy_port_map_get(unit, 0, out_port, &rcy_map_info)) {
        printf("Error: bcm_mirror_port_to_rcy_port_map_get %d\n", rv);
        return rv;
    }

    BCM_GPORT_LOCAL_SET(rcy_mirror_gport, rcy_map_info.rcy_mirror_port);

    /**
     * Set PP port CS profile of mirror RCY port to mirror_rcy_profile
     */
    rv = bcm_port_class_set(unit, rcy_mirror_gport,
                            bcmPortClassFieldIngressPMF1PacketProcessingPortCs,
                            mirror_prob_sample_info.rcy_cs_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcmPortClassFieldIngressPMF1PacketProcessingPortCs\n", rv);
        return rv;
    }

    /**
     * Set PP port key GEN var to prob for compare
     */
    if (rv = bcm_port_class_set(unit, rcy_mirror_gport, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData, prob))
    {
        printf("Error (%d), in bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData\n", rv);
        return rv;
    }

    /**
     * Set preselector for mirror RCY context
     */
    rv = mirror_egress_probabiliy_sample_presel_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mirror_egress_probabiliy_sample_presel_set\n", rv);
        return rv;
    }

    /**
     * Create probability sampling field group
     */
    rv = mirror_egress_probabiliy_sample_fg_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mirror_egress_probabiliy_sample_fg_config\n", rv);
        return rv;
    }

    /**
     * Create strip header field group
     */
    rv = mirror_egress_probabiliy_sample_strip_header_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mirror_egress_probabiliy_sample_strip_header_config\n", rv);
        return rv;
    }

    return rv;
}

/*
* Clean up procedure for mirror_egress_probabiliy_sample_example
*/
int mirror_egress_probabiliy_sample_clean_up(int unit)
{
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_pbmp_t p;
    int rv = BCM_E_NONE;

    if (*(dnxc_data_get(unit, "snif", "egress", "probability_sample", NULL)))
    {
        mirror_info.egress_sample_rate_dividend = 0;
        mirror_info.egress_sample_rate_divisor = 0;
        rv = mirror_span_example_clean_up(unit);
        set_outbound_mode(unit, 0);

        return rv;
    }

    rv = bcm_field_entry_delete(unit, mirror_prob_sample_info.strip_header_fg_id, entry_qual_info, mirror_prob_sample_info.strip_header_ent);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete strip_header\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, mirror_prob_sample_info.strip_header_fg_id, mirror_prob_sample_info.ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach eth\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = mirror_prob_sample_info.ipmf3_presel_id;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, mirror_prob_sample_info.ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy eth\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, mirror_prob_sample_info.strip_header_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete strip_header\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, mirror_prob_sample_info.prob_sample_fg_id, entry_qual_info, mirror_prob_sample_info.prob_sample_ent);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete strip_header\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, mirror_prob_sample_info.prob_sample_fg_id, mirror_prob_sample_info.ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach eth\n", rv);
        return rv;
    }

    p_id.presel_id = mirror_prob_sample_info.ipmf1_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, mirror_prob_sample_info.ipmf1_context_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_compare_destroy pair1\n", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, mirror_prob_sample_info.ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy eth\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, mirror_prob_sample_info.prob_sample_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete strip_header\n", rv);
        return rv;
    }

    rv = bcm_mirror_port_dest_delete(unit, mirror_prob_sample_info.out_port,
            BCM_MIRROR_PORT_EGRESS, mirror_prob_sample_info.mirror_dest_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mirror_port_dest_delete\n", rv);
        return rv;
    }

    rv = bcm_mirror_destination_destroy(unit, mirror_prob_sample_info.mirror_dest_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mirror_destination_destroy\n", rv);
        return rv;
    }

    rv=bcm_l2_addr_delete(unit, mirror_prob_sample_info.da, mirror_prob_sample_info.vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_l2_addr_delete\n", rv);
        return rv;
    }

    BCM_PBMP_PORT_SET(p, mirror_prob_sample_info.in_port);
    BCM_PBMP_PORT_ADD(p, mirror_prob_sample_info.out_port);

    rv=bcm_vlan_port_remove(unit, mirror_prob_sample_info.vlan, p);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_vlan_port_remove\n", rv);
        return rv;
    }

    rv=bcm_vlan_destroy(unit, mirror_prob_sample_info.vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_vlan_destroy\n", rv);
        return rv;
    }

    return rv;
}
