/*
 * $Id: cint_dnx_vswitch_vpls_gre.c
 * v 1.0 2018/12/13
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_vswitch_vpls_gre.c  Purpose: Examples for VPLSoGRE.
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
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
  cint ../../../../src/examples/dnx/vpls/cint_vpls_no_mpls.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/dnx/tunnel/cint_ip_tunnel_to_tunnel.c
  cint ../../../../src/examples/dnx/tunnel/cint_dnx_vswitch_vpls_gre.c
  cint
  int unit = 0;
  int rv = 0;
  int access_port = 200;
  int provider_port = 201;
  rv = dnx_vswitch_vpls_gre_run(unit,access_port,provider_port);
  print rv;
 * Test Scenario - end
 *
 * Scenarios configured in this cint:
 * A VPLSoGRE4oIPv4oETH tunnel header is being encapsulated or terminated.
 * For access to core, the incoming ETH packet is bridged. The outgoing packet
 * should be encapsulated as ETHoVPLSoGRE4oIPv4oETH.
 * For core to access, ETHoVPLSoGRE4oIPv4oETH incomes. it is bridged to access
 * after the GRE and VPLS label being terminated.
 * To simplify the case, static MAC is used in this example.
 */
int
dnx_vswitch_vpls_gre_ip_tunnel_params_update(
    int unit)
{
    cint_ip_tunnel_basic_info.in_tunnel_type = bcmTunnelTypeGreAnyIn4;
    cint_ip_tunnel_basic_info.out_tunnel_type = bcmTunnelTypeGreAnyIn4;

    return BCM_E_NONE;
}


int
dnx_vswitch_vpls_gre_run(
    int unit,
    int access_port,
    int provider_port)
{
    char error_msg[200]={0,};
    char *proc_name = "vpls-over-gre";
    l3_ingress_intf ingress_itf;

    BCM_IF_ERROR_RETURN_MSG(vpls_no_mpls_params_init(unit, access_port, provider_port), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_vswitch_vpls_gre_ip_tunnel_params_update(unit), "");

    /*
     * Configure AC and PWE ports
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_configure_port_properties(unit, ac_port, 0 /* create_rif */ ), "ac_port");
    BCM_IF_ERROR_RETURN_MSG(vpls_configure_port_properties(unit, pwe_port, 1 /* create_rif */ ), "pwe_port");

    /*
     * create vlan based on the vsi (vpn) 
     */
    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", vpn);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vpn), error_msg);
    pwe_term.forwarding_domain = vpn;

    /*
     * Configure L3 interfaces 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_create_l3_interfaces(unit, &pwe_port), "");

    /** Update rif with VRF, This VRF value is used for IP-Tunnel-Term.VRF lookup key. */
    sal_memset(&ingress_itf, 0, sizeof(ingress_itf));
    ingress_itf.intf_id = pwe_port.intf;
    ingress_itf.vrf     = cint_ip_tunnel_basic_info.in_provider_vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_itf), "eth_rif_intf_in_provider");

    /*
     * Configure an ARP entries
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_create_arp_entry(unit, &pwe_port), "");
    printf("%s(): created arp entry by 0x%08X\r\n",proc_name, pwe_port.arp);

    cint_ip_tunnel_basic_info.arp_id = pwe_port.arp;

    /*
     * Configure IP tunnel 
     */
    BCM_IF_ERROR_RETURN_MSG(tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &cint_ip_tunnel_basic_info.tunnel_if), "");

    /*
     * Configure fec entry 
     */
    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, ac_port.encap_fec_id, 0, cint_ip_tunnel_basic_info.tunnel_if, pwe_port.port, mpls_tunnel_in_eei?BCM_L3_ENCAP_SPACE_OPTIMIZED:0,fec_flags2),
        "create egress object FEC only");
    pwe_term.egress_tunnel_if = ac_port.encap_fec_id;

    /*
     * configure PWE tunnel - egress flow 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_mpls_port_add_encapsulation(unit, &pwe_encap), "");
    pwe_term.encap_id = pwe_encap.encap_id;

    /*
     * configure IP tunnel term- ingress flow 
     */
    BCM_IF_ERROR_RETURN_MSG(tunnel_terminator_create(unit), "");

    /*
     * configure PWE tunnel term - ingress flow 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_mpls_port_add_termination(unit, &pwe_term), "");

    /*
     * define ac and pwe ports
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_vswitch_add_access_port(unit, &ac_port, vpn), "");
    BCM_IF_ERROR_RETURN_MSG(vpls_vswitch_add_access_port(unit, &pwe_port, vpn), "");

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    /** Add l2 addresses for AC to PWE*/
    BCM_IF_ERROR_RETURN_MSG(vpls_l2_addr_add(unit, vpn, ac_port.mac_addr, pwe_term.mpls_port_id, ac_port.stat_id, ac_port.stat_pp_profile), "");

    /** Add l2 addresses for PWE to AC*/
    BCM_IF_ERROR_RETURN_MSG(vpls_l2_addr_add(unit, vpn, ac_port.next_hop, ac_port.vlan_port_id, pwe_port.stat_id, pwe_port.stat_pp_profile), "");

    if(vpls_util_verbose == 1)
    {
        printf("%s(): config complete. access port %d, provider port %d\r\n", proc_name, access_port, provider_port);
    }
    return BCM_E_NONE;
}


