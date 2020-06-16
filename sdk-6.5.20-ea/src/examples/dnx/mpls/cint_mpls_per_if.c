/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_mpls_per_if.c Purpose: MPLS lookup per-interface example
 */

/*
 *  
 * Configuration:
 *  
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c  
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c   
 * cint ../../../../src/examples/dnx/mpls/cint_mpls_per_if.c
 * cint                                                                   
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = mpls_per_if_main(unit,in_port,out_port);
 * print rv; 
 * 
 * Brief:
 * This cint define the following scenarios of processing MPLS label 6000 (LBL1):
 * 1. Send: ETH1|LBL1|IP        -->     Terminate ETH1, LBL1. Fwd IP            (Match LBL1 in ETH1 context during Tunnel Term)
 * 2. Send: ETH2|LBL1|IP        -->     Terminate ETH2, Fwd LBL1 (LSR)          (Match LBL1 in ETH2 context during FWD lookup)
 * 3. Send: ETH3|LBL2|LBL1|IP   -->     Terminate ETH3, LBL2, LBL1. Fwd IP      (Match LBL1 in LBL2 context during Tunnel Term)
 * 4. Send: ETH3|LBL3|LBL1      -->     Terminate ETH3, LBL3, Fwd LBL1 (LSR)    (Match LBL1 in LBL3 context during FWD lookup)
 * 
 *  Detailed Scenario configured in this cint:
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  1. Termination of MPLS over DA1
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One MPLS label is terminated. Lookup in routing table results in  IP forwarding.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA1     | SA           ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||Label:6000|| 10.0.0.17|| 10.161.161.162|| 64   ||
 *   |    |              |              ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||  SIP     ||  DIP          ||  TTL ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:03|| 10.0.0.17|| 10.161.161.162|| 63   ||
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  2. Switching MPLS over DA2
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One MPLS label is forwarded (LSR).
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA2     | SA           ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |0C:00:02:00:01|11:00:00:01:12||Label:6000|| 10.0.0.17|| 10.161.161.162|| 64   ||
 *   |    |              |              ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:03||Label:7000|| 10.0.0.17|| 10.161.161.162|| 64   ||
 *   |    |              |              ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  3. Termination of MPLS in the context of the previous MPLS
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One MPLS label is terminated. The next is terminated in the context of the first.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA3     | SA           ||   MPLS   ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |0C:00:02:00:02|11:00:00:01:12||Label:6001||Label:6000|| 10.0.0.17|| 10.161.161.162|| 64   ||
 *   |    |              |              ||Exp:0     ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||  SIP     ||  DIP          ||  TTL ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:03|| 10.0.0.17|| 10.161.161.162|| 63   ||
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  4. Switching MPLS in the context of the prevoius MPLS
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One MPLS label is terminated. The next is switched in the context of the first.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA3     | SA           ||   MPLS   ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |0C:00:02:00:02|11:00:00:01:12||Label:6002||Label:6000||10.0.0.17 ||10.161.161.162 || 64   ||
 *   |    |              |              ||Exp:0     ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:03||Label:7000||10.0.0.17 ||10.161.161.162 || 64   ||
 *   |    |              |              ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 */

struct cint_mpls_per_if_eth_in_rif_s
{
    int intf;
    int vid;
    bcm_mac_t mac;
};

struct cint_mpls_per_if_mpls_term_label_s
{
    int intf;
    bcm_mpls_label_t label;
};

struct cint_mpls_per_if_eth_out_rif_arp_s
{
    int arp_id;
    bcm_mac_t my_mac_out;
    bcm_mac_t next_hop_mac;
    int intf;
};

struct cint_mpls_per_if_ipv4_fwd_info_s
{
    bcm_ip_t dip;
    uint32 mask;
    int fec_id;
};

struct cint_mpls_per_if_mpls_swap_info_s
{
    bcm_mpls_label_t label;     /* Egress label after swapping */
    int fec_id;
};

struct cint_mpls_per_if_ipv4_tunnel_s
{
    bcm_ip_t tunnel_dip;
    bcm_ip_t tunnel_dip_mask;
    bcm_ip_t tunnel_sip;
    bcm_ip_t tunnel_sip_mask;
    int tunnel_type;
    bcm_if_t intf;
};

struct cint_mpls_per_if_basic_info_s
{
    int in_port;
    int out_port;
    int vrf;
    bcm_mpls_label_t processed_label;   /* LBL1 */
    cint_mpls_per_if_eth_in_rif_s eth_in_rif_1; /* ETH1 */
    cint_mpls_per_if_eth_in_rif_s eth_in_rif_2; /* ETH2 */
    cint_mpls_per_if_eth_in_rif_s eth_in_rif_3; /* ETH3 */
    cint_mpls_per_if_mpls_term_label_s term_label_2;    /* LBL2 */
    cint_mpls_per_if_mpls_term_label_s term_label_3;    /* LBL3 */
    cint_mpls_per_if_eth_out_rif_arp_s out_rif_info;
    cint_mpls_per_if_ipv4_fwd_info_s ipv4_info;
    cint_mpls_per_if_mpls_swap_info_s mpls_swap_info;
    cint_mpls_per_if_ipv4_tunnel_s ipv4_tunnel_1;
    cint_mpls_per_if_ipv4_tunnel_s ipv4_tunnel_2;
};

cint_mpls_per_if_basic_info_s cint_mpls_per_if_basic_info = {
    /*
     * in_port, out_port
     */
    200, 201,
    /*
     * vrf
     */
    100,
    /*
     * processed_label
     */
    6000,
    /*
     * eth_in_rif_1
     */
    {40, 0xa, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}},
    /*
     * eth_in_rif_2
     */
    {41, 0xb, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}},
    /*
     * eth_in_rif_3
     */
    {42, 0xc, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02}},
    /*
     * term_label_2
     */
    {0, 2000},
    /*
     * term_label_3
     */
    {0, 2001},
    /*
     * out_rif_info
     */
    {9000, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x03}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}, 120},
    /*
     * ipv4_info
     */
    {0x0AA1A1A2 /* 10.161.161.162 */ , 0xfffffff0, 50001 /* FEC-ID */ },
    /*
     * mpls_swap_info
     */
    {6777, 50003 /* FEC-ID */ },
    /*
    * ipv4_tunnel_1
    */
    {0xAB000001 /* 171.0.0.1 */, 0xffffffff, 0xAC000001 /* 172.0.0.1 */, 0x00000000, bcmTunnelTypeUdp, 0},
    /*
    * ipv4_tunnel_2
    */
    {0xAB000002 /* 171.0.0.2 */, 0xffffffff, 0xAC000002 /* 172.0.0.2 */, 0x00000000, bcmTunnelTypeUdp, 0}
};

/*
 * Main function for MPLS per I/F termination and forwarding setup
 */
int
mpls_per_if_main(
    int unit,
    int in_port,
    int out_port)
{
    int rv = BCM_E_NONE;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_mpls_per_if_basic_info.ipv4_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_mpls_per_if_basic_info.mpls_swap_info.fec_id = cint_mpls_per_if_basic_info.ipv4_info.fec_id + 1;
    /*
     * Configure port properties for this application
     */
    rv = mpls_per_if_configure_port_properties(unit, in_port, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_configure_port_properties\n", rv);
        return rv;
    }

    /*
     * Create ingress interfaces
     */
    rv = mpls_per_if_l3_ingr_interfaces_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_l3_ingr_interfaces_create\n", rv);
        return rv;
    }

    /*
     * Create out RIF + ARP for all flows
     */
    rv = mpls_per_if_out_rif_arp_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_out_rif_arp_create\n", rv);
        return rv;
    }

    /*
    * Create terminated IP tunnel
    */
    rv = mpls_per_if_udp_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_udp_terminator_create\n", rv);
        return rv;
    }

    /*
     * Setup IP forwarding
     */
    rv = mpls_per_if_ipv4_forwarding_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_ipv4_forwarding_add\n", rv);
        return rv;
    }

    /*
     * Done setting up the environment. Add all the matches of the processed label.
     */

    /*
     * Setup termination flows
     */
    rv = mpls_per_if_termination_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_termination_create\n", rv);
        return rv;
    }

    /*
     * Setup label switching for the processed label
     */
    rv = mpls_per_if_lsr_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_lsr_set\n", rv);
        return rv;
    }

    return rv;
}

int
mpls_per_if_udp_terminator_create(
    int unit)
{
    int rv = BCM_E_NONE;

    rv = mpls_per_if_udp_tunnel_terminator_create(unit, &cint_mpls_per_if_basic_info.ipv4_tunnel_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_udp_tunnel_terminator_create 1\n", rv);
        return rv;
    }

    rv = mpls_per_if_udp_tunnel_terminator_create(unit, &cint_mpls_per_if_basic_info.ipv4_tunnel_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_udp_tunnel_terminator_create 2\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Function that creates the udp tunnel terminator.
 * Inputs: unit - relevant unit;
 */
int
mpls_per_if_udp_tunnel_terminator_create(
    int unit,
    cint_mpls_per_if_ipv4_tunnel_s * ipv4_tunnel)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t intf_id;
    bcm_l3_ingress_t ing_intf;

    /** Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = ipv4_tunnel->tunnel_dip;
    tunnel_term.dip_mask = ipv4_tunnel->tunnel_dip_mask;
    tunnel_term.sip = ipv4_tunnel->tunnel_sip;
    tunnel_term.sip_mask = ipv4_tunnel->tunnel_sip_mask;
    tunnel_term.vrf = cint_mpls_per_if_basic_info.vrf;
    tunnel_term.type = ipv4_tunnel->tunnel_type;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
    bcm_l3_ingress_t_init(&ing_intf);
    ing_intf.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;        /* must, as we update exist RIF */
    ing_intf.vrf = cint_mpls_per_if_basic_info.vrf;
    /** Convert tunnel's GPort ID to intf ID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

    rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create \n");
        return rv;
    }

    ipv4_tunnel->intf = intf_id;

    printf("mpls_per_if_udp_tunnel_terminator_create:\n");
    print tunnel_term;

    return rv;
}

int
mpls_per_if_configure_port_properties(
    int unit,
    int in_port,
    int out_port)
{

    int rv = BCM_E_NONE;

    cint_mpls_per_if_basic_info.in_port = in_port;
    cint_mpls_per_if_basic_info.out_port = out_port;

    /*
     * Set In-Port to In ETh-RIF-1
     */
    rv = mpls_per_if_in_port_intf_set(unit, &cint_mpls_per_if_basic_info.eth_in_rif_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), mpls_per_if_in_port_intf_set eth_in_rif_1\n", rv);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, cint_mpls_per_if_basic_info.eth_in_rif_1.vid, cint_mpls_per_if_basic_info.in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), bcm_vlan_gport_add\n", rv);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF-2
     */
    rv = mpls_per_if_in_port_intf_set(unit, &cint_mpls_per_if_basic_info.eth_in_rif_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), mpls_per_if_in_port_intf_set eth_in_rif_2\n", rv);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, cint_mpls_per_if_basic_info.eth_in_rif_2.vid, cint_mpls_per_if_basic_info.in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), bcm_vlan_gport_add\n", rv);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF-3
     */
    rv = mpls_per_if_in_port_intf_set(unit, &cint_mpls_per_if_basic_info.eth_in_rif_3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), mpls_per_if_in_port_intf_set eth_in_rif_3\n", rv);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, cint_mpls_per_if_basic_info.eth_in_rif_3.vid, cint_mpls_per_if_basic_info.in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), bcm_vlan_gport_add\n", rv);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_mpls_per_if_basic_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), out_port_set\n", rv);
        return rv;
    }

    return rv;
}

int
mpls_per_if_in_port_intf_set(
    int unit,
    cint_mpls_per_if_eth_in_rif_s * eth_in_rif)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = cint_mpls_per_if_basic_info.in_port;
    vlan_port.vsi = eth_in_rif->intf;
    vlan_port.match_vlan = eth_in_rif->vid;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), bcm_vlan_port_create\n", rv);
        print vlan_port;
        return rv;
    }

    printf("bcm_vlan_port_create: port = %d, vlan = %d, vlan_port_id = 0x%08X\n", vlan_port.port, vlan_port.match_vlan,
           vlan_port.vlan_port_id);

    return rv;
}

/*
 * Create L3 ingress interfaces that are used as contexts to the processing of LBL1 (6000)
 */
int
mpls_per_if_l3_ingr_interfaces_create(
    int unit)
{
    int rv = BCM_E_NONE;

    /*
     * ETH1
     */
    rv = mpls_per_if_eth_in_rif_create(unit, BCM_L3_INGRESS_MPLS_INTF_NAMESPACE,
                                       cint_mpls_per_if_basic_info.eth_in_rif_1.intf,
                                       cint_mpls_per_if_basic_info.eth_in_rif_1.mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), mpls_per_if_in_rif_create\n", rv);
        print cint_mpls_per_if_basic_info.eth_in_rif_1;
        return rv;
    }

    /*
     * ETH2
     */
    rv = mpls_per_if_eth_in_rif_create(unit, BCM_L3_INGRESS_MPLS_INTF_NAMESPACE,
                                       cint_mpls_per_if_basic_info.eth_in_rif_2.intf,
                                       cint_mpls_per_if_basic_info.eth_in_rif_2.mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), mpls_per_if_in_rif_create\n", rv);
        print cint_mpls_per_if_basic_info.eth_in_rif_2;
        return rv;
    }

    /*
     * ETH3 - This interface is not used as context for MPLS lookup so no flag is set.
     */
    rv = mpls_per_if_eth_in_rif_create(unit, 0,
                                       cint_mpls_per_if_basic_info.eth_in_rif_3.intf,
                                       cint_mpls_per_if_basic_info.eth_in_rif_3.mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), mpls_per_if_in_rif_create\n", rv);
        print cint_mpls_per_if_basic_info.eth_in_rif_3;
        return rv;
    }

    /*
     * Terminated labels 2 and 3
     */

    /*
     * LBL2
     */
    rv = mpls_per_if_term_label_create(unit, BCM_L3_INGRESS_MPLS_INTF_NAMESPACE,
                                       &cint_mpls_per_if_basic_info.term_label_2.intf,
                                       cint_mpls_per_if_basic_info.term_label_2.label);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), mpls_per_if_term_label_create\n", rv);
        print cint_mpls_per_if_basic_info.term_label_2;
        return rv;
    }

    /*
     * LBL3
     */
    rv = mpls_per_if_term_label_create(unit, BCM_L3_INGRESS_MPLS_INTF_NAMESPACE,
                                       &cint_mpls_per_if_basic_info.term_label_3.intf,
                                       cint_mpls_per_if_basic_info.term_label_3.label);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), mpls_per_if_term_label_create\n", rv);
        print cint_mpls_per_if_basic_info.term_label_3;
        return rv;
    }

    return rv;
}

/*
 * Create an In-RIF
 */
int
mpls_per_if_eth_in_rif_create(
    int unit,
    uint32 flags,
    bcm_if_t intf,
    bcm_mac_t mac)
{
    int rv = BCM_E_NONE;
    l3_ingress_intf ingr_itf;

    /*
     * Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf, mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), intf_eth_rif_create\n", rv);
        print mac;
        print intf;
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.flags = flags;
    ingr_itf.intf_id = intf;
    ingr_itf.vrf = cint_mpls_per_if_basic_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), intf_ingress_rif_set\n", rv);
        print ingr_itf;
        return rv;
    }

    printf("ETH RIF created: intf = 0x%x, ", ingr_itf.intf_id);
    print mac;

    return rv;
}

/*
 * Create a terminated MPLS label LIF and set up an L3 ingress interface for it.
 */
int
mpls_per_if_term_label_create(
    int unit,
    uint32 flags,
    int *intf,
    bcm_mpls_label_t label)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;
    l3_ingress_intf ingr_itf;

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.label = label;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }

    /*
     * Set Incoming Tunnel-IF properties
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.flags = flags;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
    ingr_itf.intf_id = l3_intf_id;
    ingr_itf.vrf = cint_mpls_per_if_basic_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), intf_ingress_rif_set\n", rv);
        return rv;
    }

    *intf = ingr_itf.intf_id;

    printf("Terminated label %d (0x%05x) created: tunnel_id = 0x%x, intf = 0x%x\n",
           entry.label, entry.label, entry.tunnel_id, ingr_itf.intf_id);

    return rv;
}

/*
 * Create RIF and ARP entry for egress packets after routing.
 */
int
mpls_per_if_out_rif_arp_create(
    int unit)
{
    int rv = BCM_E_NONE;

    /*
     * Setup out-RIF
     */
    rv = intf_eth_rif_create(unit, cint_mpls_per_if_basic_info.out_rif_info.intf,
                             cint_mpls_per_if_basic_info.out_rif_info.my_mac_out);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), intf_eth_rif_create\n", rv);
        return rv;
    }

    /*
     * Configure ARP entry
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_mpls_per_if_basic_info.out_rif_info.arp_id),
                                       cint_mpls_per_if_basic_info.out_rif_info.next_hop_mac,
                                       cint_mpls_per_if_basic_info.out_rif_info.intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), l3__egress_only_encap__create\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Add DIP to routing table of the VRF to send IPv4 routed packets to the egress I/F
 */
int
mpls_per_if_ipv4_forwarding_add(
    int unit)
{
    int rv = BCM_E_NONE;
    /*
     * Create FEC for termination scenario;
     * 1) Give a fec id.
     * 2) Give the out rif.
     * 3) Give the arp id.
     * 4) Give the out port.
     */
    rv = l3__egress_only_fec__create(unit, cint_mpls_per_if_basic_info.ipv4_info.fec_id,
                                     cint_mpls_per_if_basic_info.out_rif_info.intf,
                                     cint_mpls_per_if_basic_info.out_rif_info.arp_id,
                                     cint_mpls_per_if_basic_info.out_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), l3__egress_only_fec__create\n", rv);
        return rv;
    }
    /*
     * Create a routing entry :
     * 1) Give the Destination IP (the ip in the exposed header after termination)
     * 2) Give the mask for the sub net of the DIP
     * 3) Give VRF
     * 4) Give the fec id associated with this forwarding instance.
     */
    rv = add_host_ipv4(unit, cint_mpls_per_if_basic_info.ipv4_info.dip, cint_mpls_per_if_basic_info.vrf, cint_mpls_per_if_basic_info.ipv4_info.fec_id, 0        /* arp 
                                                                                                                                                                 * id 
                                                                                                                                                                 */ , 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), add_route_ipv4\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Setup termination of LBL1 when it is matched in the context of the interfaces ETH1 and LBL2
 * (Scenarios 1, 3)
 */
int
mpls_per_if_termination_create(
    int unit)
{
    int rv = BCM_E_NONE;

    /*
     * Create termination of processed label in the context of ETH1
     */
    rv = mpls_per_if_processed_label_termination_create(unit, cint_mpls_per_if_basic_info.eth_in_rif_1.intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_processed_label_termination_create\n", rv);
        return rv;
    }
    /*
     * Create termination of processed label in the context of LBL2
     */
    rv = mpls_per_if_processed_label_termination_create(unit, cint_mpls_per_if_basic_info.term_label_2.intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_processed_label_termination_create\n", rv);
        return rv;
    }
    /*
     * Create termination of processed label in the context of IPv4_tunnel_1
     */
    rv = mpls_per_if_processed_label_termination_create(unit, cint_mpls_per_if_basic_info.ipv4_tunnel_1.intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_processed_label_termination_create\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Create termination of LBL1 in the context of a previous interface
 */
int
mpls_per_if_processed_label_termination_create(
    int unit,
    int context_intf)
{
    int rv = BCM_E_NONE;;
    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;
    l3_ingress_intf ingr_itf;

    /*
     * Tunnel creation
     */
    bcm_mpls_tunnel_switch_t_init(&entry);
    /*
     * Indicate termination
     */
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    /*
     * indicate lookup in ingress interface context
     */
    entry.flags |= BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF;
    /*
     * Label to be terminated
     */
    entry.label = cint_mpls_per_if_basic_info.processed_label;
    /*
     * Context interface
     */
    entry.ingress_if = context_intf;
    /*
     * This is the main API that affects this example. Instead of adding a match of the label alone,
     * it will add a match of the label with the supplied interface.
     */
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }
    /*
     * Set Incoming Tunnel-RIF properties
     */
    l3_ingress_intf_init(&ingr_itf);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
    ingr_itf.intf_id = l3_intf_id;
    ingr_itf.vrf = cint_mpls_per_if_basic_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), intf_ingress_rif_set\n", rv);
        return rv;
    }

    printf
        ("Processed Label, %d (0x%05x) termination added: context intf = 0x%x, tunnel_id = 0x%x, tunnel_intf = 0x%x\n",
         entry.label, entry.label, entry.ingress_if, entry.tunnel_id, ingr_itf.intf_id);

    return rv;
}

/*
 * Setup forwarding of LBL1 when it is matched in the context of ETH2 or LBL3
 * (Scenarios 2, 4)
 */
int
mpls_per_if_lsr_set(
    int unit)
{
    int rv = BCM_E_NONE;

    /*
     * Create tunnel initiator and FEC that points to it for use in swapping the processed label
     */
    rv = mpls_per_if_swap_label_info_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_swap_label_info_create\n");
        return rv;
    }

    /*
     * Create fwd of processed label in the context of ETH2
     */
    rv = mpls_per_if_processed_label_switch_create(unit, cint_mpls_per_if_basic_info.eth_in_rif_2.intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_processed_label_switch_create rif_2\n", rv);
        return rv;
    }
    /*
     * Create fwd of processed label in the context of LBL3
     */
    rv = mpls_per_if_processed_label_switch_create(unit, cint_mpls_per_if_basic_info.term_label_3.intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_processed_label_switch_create label_3\n", rv);
        return rv;
    }
    /*
     * Create fwd of processed label in the context of IPv4_tunnel_2
     */
    rv = mpls_per_if_processed_label_switch_create(unit, cint_mpls_per_if_basic_info.ipv4_tunnel_2.intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_per_if_processed_label_switch_create label_3\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Creates a tunnel initiator with the swapped label, pointing to the arp entry.
 * And a fec pointing to it and to the out port.
 */
int
mpls_per_if_swap_label_info_create(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t eg_label;

    bcm_mpls_egress_label_t_init(&eg_label);
    eg_label.label = cint_mpls_per_if_basic_info.mpls_swap_info.label;
    eg_label.l3_intf_id = cint_mpls_per_if_basic_info.out_rif_info.arp_id;
    eg_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    eg_label.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    eg_label.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0 /* intf, must be 0 */ , 1, &eg_label);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_initiator_create\n", rv);
        print eg_label;
        return rv;
    }

    printf("Tunnel initiator created for MPLS swapping. Label = %d (0x%05x), LIF = 0x%x\n",
           eg_label.label, eg_label.label, eg_label.tunnel_id);

    /*
     * Create FEC for forward scenario;
     * 1) Give a fec id.
     * 2) Give the out lif.
     * 3) Arp nulled because this is encap_only.
     * 4) Give the out port.
     */
    rv = l3__egress_only_fec__create(unit, cint_mpls_per_if_basic_info.mpls_swap_info.fec_id,
                                     eg_label.tunnel_id, 0, cint_mpls_per_if_basic_info.out_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), l3__egress_only_fec__create\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Create forwarding of LBL1 in the context of a previous interface
 */
int
mpls_per_if_processed_label_switch_create(
    int unit,
    int context_intf)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * Tunnel creation
     */
    bcm_mpls_tunnel_switch_t_init(&entry);
    /*
     * indicate lookup in ingress interface context
     */
    entry.flags |= BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF;
    /*
     * Label to be matched
     */
    entry.label = cint_mpls_per_if_basic_info.processed_label;
    /*
     * Context interface
     */
    entry.ingress_if = context_intf;
    /*
     * Actions to perform (ingress)
     */
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    /*
     * Setup forwarding information
     */
    BCM_GPORT_FORWARD_PORT_SET(entry.port, cint_mpls_per_if_basic_info.mpls_swap_info.fec_id);
    /*
     * This is the main API that affects this example. Instead of adding a match of the label alone,
     * it will add a match of the label with the supplied interface.
     */
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }

    printf("Processed Label, %d (0x%05x) in context intf 0x%x, forwarding added to gport = 0x%x\n",
           entry.label, entry.label, entry.ingress_if, entry.port);

    return rv;
}
