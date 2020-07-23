/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_ip_tunnel_to_tunnel_basic.c Purpose: Example of basic IP tunnel to tunnel configuration.
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
 * cint ../../../../src/examples/dnx/cint_ip_tunnel_to_tunnel_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_provider_port = 200;
 * int out_provider_port = 201;
 * rv = ip_tunnel_to_tunnel_basic(unit, in_provider_port, out_provider_port);
 * print rv;
 *
 *
 *  Scenarios configured in this cint:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) Basic IP Tunnel to Tunnel
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE4oIPv4 tunnel header is being terminated and VXLAN-GPEoUDP tunnel is encpasulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################################
 *  Sending packet from  in provider port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||  GRE  ||      IPv4 SIP     ||      IPv4 DIP      ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||    172.0.0.1    ||    171.0.0.1    ||       ||  160.161.161.162  ||    192.128.1.1     ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on out provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   |    |      DA      | SA           || IPv4 tunnel SIP || IPv4 tunnel DIP ||  UDP dest port:  ||         VXLAN-GPE         ||      IPv4 SIP     ||      IPv4 DIP      ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||    182.0.0.1    ||    181.0.0.1    ||     0x5555       ||  VNI flag=1, VNI=0x1388   ||  160.161.161.162  ||    192.128.1.1     ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ################################################################################################################################
 */
struct cint_ip_tunnel_basic_info_s
{
    int eth_rif_intf_in_provider;               /* in RIF */
    int eth_rif_intf_out_provider;              /* out RIF */
    int tunnel_if;                              /* RIF, property of the tunnel termination inLif.*/
    bcm_mac_t intf_in_provider_mac_address;     /* mac for in RIF */
    bcm_mac_t intf_out_provider_mac_address;    /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;                 /* mac for next hop */
    int out_provider_vrf;                       /* router instance VRF resolved at VTT1*/
    int in_provider_vrf;                        /* VRF after termination, resolved as Tunnel-InLif property */
    int fec_id;                                 /* FEC id */
    int arp_id;                                 /* Id for ARP entry */
    bcm_ip_t route_dip;                         /* Route DIP */
    uint32 mask;                                /* mask for route DIP */
    bcm_ip_t in_tunnel_dip;                     /* tunnel DIP */
    bcm_ip_t in_tunnel_dip_mask;                /* tunnel DIP mask*/
    bcm_ip_t in_tunnel_sip;                     /* tunnel SIP */
    bcm_ip_t in_tunnel_sip_mask;                /* tunnel SIP mask */
    int tunnel_type_in_provider;                /* bcmTunnelTypeGreAnyIn4 */
    int tunnel_type_out_provider;               /* bcmTunnelTypeVxlan */
    bcm_ip_t out_tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t out_tunnel_dip_mask;               /* tunnel DIP mask*/
    bcm_ip_t out_tunnel_sip;                    /* tunnel SIP */
    bcm_ip_t out_tunnel_sip_mask;               /* tunnel SIP mask */
    int out_provider_vlan;                      /* VID assigned to the outgoing packet */
    int out_tunnel_ttl;                         /* Ip tunnel header TTL */
    int out_tunnel_dscp;                        /* Ip tunnel header differentiated services code point */
};

cint_ip_tunnel_basic_info_s cint_ip_tunnel_basic_info =
{
    /*
     * eth_rif_intf_in_provider | eth_rif_intf_out_provider | tunnel_if
     */
    15, 100, 20,
    /*
     * intf_in_provider_mac_address | intf_out_provider_mac_address | arp_next_hop_mac |
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * out_provider_vrf VRF, in_provider_vrf VRF
     */
    5, 1,
    /*
     * fec_id
     */
    40961,
    /*
     * arp_id
     */
    0,
    /*
     * route_dip
     */
    0xA0A1A1A2 /* 160.161.161.162 */ ,
    /*
     * mask
     */
    0xfffffff0,
    /*
     * in_tunnel DIP
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * in_tunnel DIP mask
     */
    0xffffffff,
    /*
     * in_tunnel SIP
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * in_tunnel SIP mask
     */
    0xffffffff,
    /*
     * bcmTunnelType - incoming, bcmTunnelType - outgoing
     */
    bcmTunnelTypeGreAnyIn4, bcmTunnelTypeVxlanGpe,
    /*
     * out_tunnel DIP
     */
    0xB5000001, /* 181.0.0.1 */
    /*
     * out_tunnel DIP mask
     */
    0xffffffff,
    /*
     * out_tunnel SIP
     */
    0xB6000001, /* 182.0.0.1 */
    /*
     * out_tunnel SIP mask
     */
    0xffffffff,
    /*
     * out provider vlan
     */
       100,
    /*
     * tunnel_ttl
     */
       64,
    /*
     * tunnel_dscp
     */
       10
};

bcm_if_t g_intf_id;

/*
* Main function for basic ip tunnel to tunnel scenario in Jericho 2
* The function ip_tunnel_to_tunnel_basic implements the basic tunnel to tunnel scenario.
* Inputs:
* unit - relevant unit;
* in_provider_port - incoming port;
* out_provider_port - outgoing port;
*/
int
ip_tunnel_to_tunnel_basic(
    int unit,
    int in_provider_port,
    int out_provider_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    bcm_if_t tunnel_intf;
    char *proc_name;

    proc_name = "ip_tunnel_to_tunnel_basic";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_provider_port, cint_ip_tunnel_basic_info.eth_rif_intf_in_provider);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set eth_rif_intf_in_provider\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, out_provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set out_provider_port\n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the in_tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_in_provider, cint_ip_tunnel_basic_info.intf_in_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_in_provider\n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_out_provider, cint_ip_tunnel_basic_info.intf_out_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_out_provider\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_in_provider;
    ingr_itf.vrf = cint_ip_tunnel_basic_info.in_provider_vrf;

    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_intf_in_provider\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_terminator_configure \n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_id), cint_ip_tunnel_basic_info.arp_next_hop_mac, cint_ip_tunnel_basic_info.out_provider_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     * and set next-lif-pointer to the ARP entry
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port
     */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, tunnel_intf, 0, out_provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
    }

    /*
     * Add Host entry
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.fec_id);
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip, cint_ip_tunnel_basic_info.out_provider_vrf, fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }
    rv = vxlan_network_domain_management_l3_egress(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function vxlan_network_domain_management_l3_egress, \n");
        return rv;
    }
    return rv;
}

int tunnel_terminator_create(
                        int unit)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_ip_tunnel_basic_info.in_tunnel_dip;
    tunnel_term.dip_mask = cint_ip_tunnel_basic_info.in_tunnel_dip_mask;
    tunnel_term.sip = cint_ip_tunnel_basic_info.in_tunnel_sip;
    tunnel_term.sip_mask = cint_ip_tunnel_basic_info.in_tunnel_sip_mask;
    tunnel_term.vrf = cint_ip_tunnel_basic_info.in_provider_vrf;
    tunnel_term.type = cint_ip_tunnel_basic_info.tunnel_type_in_provider;
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
     /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
     bcm_l3_ingress_t ing_intf;
     bcm_l3_ingress_t_init(&ing_intf);

     ing_intf.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
     ing_intf.vrf = cint_ip_tunnel_basic_info.out_provider_vrf;

     /* Convert tunnel's GPort ID to intf ID */
      BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(g_intf_id, tunnel_term.tunnel_id);

      rv = bcm_l3_ingress_create(unit, ing_intf, g_intf_id);
      if (rv != BCM_E_NONE) 
      {
          printf("Error, bcm_l3_ingress_create\n");
          return rv;
      }

    return rv;
}

int tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t *tunnel_intf)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    bcm_l3_intf_t_init(&l3_intf);

    /* Create IP tunnel initiator for encapsulating VXLAN-GPE tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = cint_ip_tunnel_basic_info.out_tunnel_dip;
    tunnel_init.sip        = cint_ip_tunnel_basic_info.out_tunnel_sip;
    tunnel_init.flags      = 0;
    tunnel_init.dscp       = cint_ip_tunnel_basic_info.out_tunnel_dscp;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type       = cint_ip_tunnel_basic_info.tunnel_type_out_provider;
    tunnel_init.ttl        = cint_ip_tunnel_basic_info.out_tunnel_ttl;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    *tunnel_intf = l3_intf.l3a_intf_id;

    return rv;
}

int
vxlan_network_domain_management_l3_egress(
    int unit)
{
    int rv;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_ip_tunnel_basic_info.out_provider_vrf;
    config.vni              = 5000;
    config.network_domain   = 0;

    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}
