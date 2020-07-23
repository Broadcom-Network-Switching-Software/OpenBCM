/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_vpls_no_mpls.c Purpose: utility for VPLS without MPLS.
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
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dnx/vpls/cint_vpls_no_mpls.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = vpls_no_mpls_main(unit,in_port,out_port);
 * print rv; 
 * 
 * 
 
 *  Scenario configured in this cint:
 *
 *  Bridge into a PWE  core. 
 *  Exit with a packet including PWE label and no MPLS.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS Termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One PWE label.
 *  Lookup in MAC table for ethernet forwarding.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  PWE         ||      DA              |     SA                || vlan | tpid
 *   |    |0C:00:02:00:01   |00:00:00:cd:1d   ||Label:3456  ||0c:00:02:00:00   |11:00:00:01:12    || 5    | 0x8100
 *   |    |                         |                       ||Exp:0     ||                  ||                  |                               ||
 *   |    |                         |                       ||TTL:20    ||                  ||                  |                           ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan | tpid
 *   |    |0c:00:02:00:00   |11:00:00:01:12   || 5    | 0x8100
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Bridge into a PWE core.
 *  Exit with a packet including PWE label.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan | tpid
 *   |    |11:00:00:01:12   |0c:00:02:00:00   || 5    | 0x8100
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  PWE         ||      DA              |     SA                || vlan | tpid
 *   |    |00:00:00:cd:1d   |0C:00:02:00:01   ||Label:3456  || 11:00:00:01:12   |0c:00:02:00:00  || 5 | 0x8100
 *   |    |                         |                       ||Exp:0     ||                  ||               ||      ||
 *   |    |                         |                       ||TTL:20    ||                  ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
*/
int vpls_no_mpls_params_set = 0;

int
vpls_no_mpls_params_init(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    char* proc_name = "vpls_no_mpls_params_init";

    rv = init_default_vpls_params(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error(%d), in init_default_vpls_params\n", proc_name, rv);
        return rv;
    }

    /* init MPLS configurations to be skipped  */
    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel);
    mpls_tunnel_switch_create_s_init(mpls_term_tunnel);

    ac_port.port = port1;
    pwe_port.port = port2;

    vpls_no_mpls_params_set = 1;

    return rv;
}

int
vpls_no_mpls_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "vpls_no_mpls_main";

    rv = vpls_no_mpls_params_init(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error(%d), in vpls_no_mpls_params_init\n", proc_name, rv);
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

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_arp_entry\n", rv);
        return rv;
    }
    printf("%s(): Going to load mpls_encap_tunnel[%d].l3_intf_id by 0x%08X\r\n",proc_name, outer_mpls_tunnel_index, pwe_port.arp);
    mpls_encap_tunnel[outer_mpls_tunnel_index].l3_intf_id = pwe_port.arp;

    /*
     * Configure fec entry 
     */
    rv = l3__egress_only_fec__create(unit, ac_port.encap_fec_id, 0, pwe_port.arp, pwe_port.port, 0);
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
     * configure PWE tunnel - ingress flow 
     */
    rv = vpls_mpls_port_add_termination(unit, &pwe_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_termination\n", rv);
        return rv;
    }

    /** define ac and pwe ports */
    rv = vpls_vswitch_add_access_port(unit, &ac_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_vswitch_add_access_port\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    rv = vpls_l2_addr_add(unit, vpn, ac_port.mac_addr, pwe_term.mpls_port_id, ac_port.stat_id, ac_port.stat_pp_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_l2_addr_add\n", rv);
        return rv;
    }
    rv = vpls_l2_addr_add(unit, vpn, ac_port.next_hop, ac_port.vlan_port_id, pwe_port.stat_id, pwe_port.stat_pp_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_l2_addr_add\n", rv);
        return rv;
    }

    if(vpls_util_verbose == 1)
    {
        printf("%s(): EXIT. port1 %d,port2 %d\r\n", proc_name, port1, port2);
    }
    return rv;
}
