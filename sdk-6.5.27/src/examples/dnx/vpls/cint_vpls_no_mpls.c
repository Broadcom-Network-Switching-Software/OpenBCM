/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_vpls_no_mpls.c Purpose: utility for VPLS without MPLS.
 */

/*
 * 
 * Configuration:
 * 
 * Test Scenario - start
 * cint;
 * cint_reset();
 * exit;
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/cint_ip_route_basic.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
  cint ../../../../src/examples/dnx/cint_dnx_advanced_vlan_translation_mode.c
  cint ../../../../src/examples/dnx/vpls/cint_vpls_no_mpls.c
  cint
  int unit = 0; 
  int rv = 0; 
  int in_port = 200; 
  int out_port = 201;
  rv = vpls_no_mpls_main(unit,in_port,out_port);
  print rv; 
 * Test Scenario - end
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

    BCM_IF_ERROR_RETURN_MSG(init_default_vpls_params(unit), "");

    /* init MPLS configurations to be skipped  */
    vpls_mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel);
    vpls_mpls_tunnel_switch_create_s_init(mpls_term_tunnel);

    ac_port.port = port1;
    pwe_port.port = port2;

    vpls_no_mpls_params_set = 1;

    return BCM_E_NONE;
}

int
vpls_no_mpls_main(
    int unit,
    int port1,
    int port2)
{
    char error_msg[200]={0,};
    char *proc_name;
    proc_name = "vpls_no_mpls_main";
    int flags2 = 0;
    BCM_IF_ERROR_RETURN_MSG(vpls_no_mpls_params_init(unit, port1, port2), "");

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

    /*
     * Configure an ARP entries
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_create_arp_entry(unit, &pwe_port), "");
    printf("%s(): Going to load mpls_encap_tunnel[%d].l3_intf_id by 0x%08X\r\n",proc_name, outer_mpls_tunnel_index, pwe_port.arp);
    mpls_encap_tunnel[outer_mpls_tunnel_index].l3_intf_id = pwe_port.arp;

    /*
     * Configure fec entry
     */
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, ac_port.encap_fec_id, 0, pwe_port.arp, pwe_port.port, mpls_tunnel_in_eei?BCM_L3_ENCAP_SPACE_OPTIMIZED:0,flags2),
        "create egress object FEC only");
    pwe_term.egress_tunnel_if = ac_port.encap_fec_id;

    /*
     * configure PWE tunnel - egress flow 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_mpls_port_add_encapsulation(unit, &pwe_encap), "");
    pwe_term.encap_id = pwe_encap.encap_id;

    /*
     * configure PWE tunnel - ingress flow 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_mpls_port_add_termination(unit, &pwe_term), "");

    /** define ac and pwe ports */
    BCM_IF_ERROR_RETURN_MSG(vpls_vswitch_add_access_port(unit, &ac_port, vpn), "");

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_l2_addr_add(unit, vpn, ac_port.mac_addr, pwe_term.mpls_port_id, ac_port.stat_id, ac_port.stat_pp_profile), "");
    BCM_IF_ERROR_RETURN_MSG(vpls_l2_addr_add(unit, vpn, ac_port.next_hop, ac_port.vlan_port_id, pwe_port.stat_id, pwe_port.stat_pp_profile), "");

    if(vpls_util_verbose == 1)
    {
        printf("%s(): EXIT. port1 %d,port2 %d\r\n", proc_name, port1, port2);
    }
    return BCM_E_NONE;
}
