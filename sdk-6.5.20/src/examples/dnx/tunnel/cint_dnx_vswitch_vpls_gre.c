/*
 * $Id: cint_dnx_vswitch_vpls_gre.c
 * v 1.0 2018/12/13
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_vswitch_vpls_gre.c  Purpose: Examples for VPLSoGRE.
 */

/*
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/dnx/vpls/cint_vpls_no_mpls.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/tunnel/cint_ip_tunnel_to_tunnel.c
 * cint ../../../../src/examples/dnx/tunnel/cint_dnx_vswitch_vpls_gre.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int access_port = 200;
 * int provider_port = 201;
 * rv = dnx_vswitch_vpls_gre_run(unit,access_port,provider_port);
 * print rv;
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
    int rv = BCM_E_NONE;
    char *proc_name = "vpls-over-gre";
    l3_ingress_intf ingress_itf;

    if (!is_device_or_above(unit, JERICHO2))
    {
        printf("Error, Not support. Refer to cint_vswitch_vpls_gre.c instead for JR1.\n");
        return BCM_E_UNAVAIL;
    }

    rv = vpls_no_mpls_params_init(unit, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error(%d), in vpls_no_mpls_params_init\n", proc_name, rv);
        return rv;
    }
    rv = dnx_vswitch_vpls_gre_ip_tunnel_params_update(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error(%d), in dnx_vswitch_vpls_gre_ip_tunnel_paras_update\n", proc_name, rv);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_configure_port_properties(unit, ac_port, 0 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties ac_port\n", rv);
        return rv;
    }
    rv = vpls_configure_port_properties(unit, pwe_port, 1 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties pwe_port\n", rv);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn) 
     */
    rv = bcm_vlan_create(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, vpn);
        return rv;
    }
    pwe_term.forwarding_domain = vpn;

    /*
     * Configure L3 interfaces 
     */
    rv = vpls_create_l3_interfaces(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_l3_interfaces\n", rv);
        return rv;
    }

    /** Update rif with VRF, This VRF value is used for IP-Tunnel-Term.VRF lookup key. */
    sal_memset(&ingress_itf, 0, sizeof(ingress_itf));
    ingress_itf.intf_id = pwe_port.intf;
    ingress_itf.vrf     = cint_ip_tunnel_basic_info.in_provider_vrf;
    rv = intf_ingress_rif_set(unit, &ingress_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_intf_in_provider\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_arp_entry\n", rv);
        return rv;
    }
    printf("%s(): created arp entry by 0x%08X\r\n",proc_name, pwe_port.arp);

    cint_ip_tunnel_basic_info.arp_id = pwe_port.arp;

    /*
     * Configure IP tunnel 
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.arp_id, &cint_ip_tunnel_basic_info.tunnel_if);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /*
     * Configure fec entry 
     */
    rv = l3__egress_only_fec__create(unit, ac_port.encap_fec_id, 0, cint_ip_tunnel_basic_info.tunnel_if, pwe_port.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    pwe_term.egress_tunnel_if = ac_port.encap_fec_id;

    /*
     * configure PWE tunnel - egress flow 
     */
    rv = vpls_mpls_port_add_encapsulation(unit, &pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_encapsulation\n", rv);
        return rv;
    }
    pwe_term.encap_id = pwe_encap.encap_id;

    /*
     * configure IP tunnel term- ingress flow 
     */
    rv = tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel term - ingress flow 
     */
    rv = vpls_mpls_port_add_termination(unit, &pwe_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_termination\n", rv);
        return rv;
    }

    /*
     * define ac and pwe ports
     */
    rv = vpls_vswitch_add_access_port(unit, &ac_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_vswitch_add_access_port\n", rv);
        return rv;
    }
    rv = vpls_vswitch_add_access_port(unit, &pwe_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_vswitch_add_access_port\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    /** Add l2 addresses for AC to PWE*/
    rv = vpls_l2_addr_add(unit, vpn, ac_port.mac_addr, pwe_term.mpls_port_id, ac_port.stat_id, ac_port.stat_pp_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_l2_addr_add\n", rv);
        return rv;
    }

    /** Add l2 addresses for PWE to AC*/
    rv = vpls_l2_addr_add(unit, vpn, ac_port.next_hop, ac_port.vlan_port_id, pwe_port.stat_id, pwe_port.stat_pp_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_l2_addr_add\n", rv);
        return rv;
    }

    if(vpls_util_verbose == 1)
    {
        printf("%s(): config complete. access port %d, provider port %d\r\n", proc_name, access_port, provider_port);
    }
    return rv;
}


