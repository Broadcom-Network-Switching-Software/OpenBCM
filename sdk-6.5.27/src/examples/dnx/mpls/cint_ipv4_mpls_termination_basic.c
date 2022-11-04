/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_mpls_basic.c Purpose: Various examples for MPLS/VPLS. 
 */

/*
 *  
 * Configuration:
 *  
 * Test Scenario - start
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c  
  cint ../../../../src/examples/sand/cint_ip_route_basic.c   
  cint ../../../../src/examples/sand/cint_mpls_termination_basic.c
  cint ../../../../src/examples/dnx/cint_ipv4_mpls_termination_basic.c
  cint                                                                   
  int unit = 0; 
  int rv = 0; 
  int in_port = 200; 
  int out_port = 201;
  rv = ipv4_fwd_ipv4_mpls_termination(unit,in_port,out_port);
  print rv; 
 * Test Scenario - end
 * 
 *  Scenario configured in this cint:
 * 
 *  One MPLS label and IPv4 header is terminated. Lookup in routing table results in IP forwarding.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||   MPLS   ||  SIP     ||  DIP          || IPv4 DIP        || IPv4 SIP ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||Label:6000||128.0.0.1 ||128.128.128.1  || 160.161.161.162 ||          ||
 *   |    |              |              ||Exp:0     ||          ||               ||                 ||          ||
 *   |    |              |              ||TTL:20    ||          ||               ||                 ||          ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||  SIP     ||  DIP          || IPv4 SIP ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||160.0.0.17||160.161.161.162||          ||
 *   |    |              |              ||          ||               ||          ||
 *   |    |              |              ||          ||               ||          ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 * 
 */

/*
 * Main function for basic mpls termination scnenario in Jericho 2: Terminate up to 4 labels
 */
int
ipv4_fwd_ipv4_mpls_termination(
    int unit,
    int port1,
    int port2)
{
    int is_qux;
 
    cint_termination_mpls_basic_info.in_port = port1;
    cint_termination_mpls_basic_info.out_port = port2;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_termination_mpls_basic_info.fec_id), "");


    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /*
     * Configure port properties for this application 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_configure_port_properties(unit), "");

    /*
     * Configure L3 interfaces 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_l3_interfaces(unit), "");

    /*
     * Configure an ARP entry 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_arp_entry(unit), "");

    /*
     * Configure fec entries for all scenarios 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_fec_entry(unit), "");

    /*
     * Configure a termination stack  
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_termination_stack(unit, 1), "");
    /** Create VXLAN-GPE IP tunnel terminators */
    cint_ip_tunnel_basic_info.provider_vrf = 100;
    cint_ip_tunnel_basic_info.tunnel_dip = 0x80808001;
    cint_ip_tunnel_basic_info.tunnel_dip_mask = 0xFFFFFFFF;
    cint_ip_tunnel_basic_info.tunnel_sip = 0x80000001;
    cint_ip_tunnel_basic_info.tunnel_sip_mask = 0xFFFFFFFF;
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeIpAnyIn4;
    int tunnel_intf_v4 = 0;
    int tunnel_id_v4 = 0;


    BCM_IF_ERROR_RETURN_MSG(tunnel_terminator_create(unit, &tunnel_intf_v4, &tunnel_id_v4), "");

    /*
     * Create a l3 forwarding entry 
     */
    int vrf = cint_ip_tunnel_basic_info.access_vrf;
    uint32 dip = 0xa0a1a1a2;
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, dip, vrf, cint_termination_mpls_basic_info.fec_id, 0, 0), "");
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_l3_forwarding(unit), "");
    return BCM_E_NONE;
}
