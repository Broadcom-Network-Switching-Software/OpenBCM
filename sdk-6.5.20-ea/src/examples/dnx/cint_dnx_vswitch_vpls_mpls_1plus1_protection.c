/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_dnx_vswitch_vpls_mpls_1plus1_protection.c
 * Purpose: Example of MC in VPLS VSwitch with Protection
 */

/*
 * File: cint_sand_vswitch_vpls.c
 * Purpose: Example of VPN flooding domain setup with one AC port and two PWE ports with VPLS and MPLS Ingress 1+1 protection.
 *
 * This example creates a VPN, a logical AC port, two logical PWE ports with VPLS and MPLS Ingress 1+1 protection and adds all the logical ports to the VPN.
 * See figure below:
 * +-------------------------------------------------------------------------------------------------------+
 * |                                                                  Native ETH VID: 100                  |
 * |                                                                  VC label: 3456                       |
 * |                                                                  MPLS label: 3333                     |
 * |                                                                  VID: 30                              |
 * |                                +----------------+                RIF MAC:  00:0C:00:02:00:01          |
 * |                                |                |                Next Hop: 00:00:00:00:CD:1D          |
 * |                                |                <-------------------->   PWE (Protection)             |
 * |                                |                |                                                     |
 * |      VID: 100       AC <------->    VSwitch     |                                                     |
 * |                                |    VPN: 100    |                                                     |
 * |                                |                <-------------------->   PWE (Primary)                |
 * |                                |                |                 Native ETH VID 100                  |
 * |                                +----------------+                 VC label: 3457                      |
 * |                                                                   MPLS label: 3334                    |
 * |                                                                   MPLS label: 3335                    |
 * |                                                                   VID: 31                             |
 * |                                                                   RIF MAC:  00:0C:00:02:00:02         |
 * |                                                                   Next Hop: 00:00:00:00:CD:1E         |
 * +-------------------------------------------------------------------------------------------------------+
 *
 * For direction AC <-- PWE: If packet is sent from one of the PWEs and the destination is the AC it should be forwarded if the failover path 
 * is set to the corresponding state (primary or protection), or it should be dropped if the failover path ais set to a state that does not correspond to the one in the In-LIF.
 *
 * For direction AC --> PWE: If the packet is sent from the AC to one of the PWEs it should be multicasted to both using the Failover MC group.
 *
 * Usage:
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/sand/cint_sand_vswitch_vpls.c
 * cint ../../../../src/examples/dnx/cint_dnx_vswitch_vpls_mpls_1plus1_protection.c
 * cint
 * print vswitch_vpls_mpls_1plus1_protection_main(0,200,201,202);
 */

bcm_mac_t mac = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x06 };
bcm_failover_t  failover_id_vpls = 0x0003;
bcm_failover_t  failover_id_mpls = 0x0002;
bcm_multicast_t  failover_mc_group = 234;

int
vswitch_vpls_mpls_1plus1_protection_multicast_replications_add(
    int unit,
    bcm_multicast_t failover_mc_group,
    bcm_port_t port,
    bcm_gport_t mpls_port_id)
{
    int rv = BCM_E_NONE;
    bcm_if_t encapl_id;
    bcm_multicast_replication_t replication;

    /*
     * Get encap ID for primary PWE
     */
    rv = bcm_multicast_vpls_encap_get(unit, failover_mc_group, port, mpls_port_id, &encapl_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_multicast_vpls_encap_get\n", rv);
        return rv;
    }


    /*
     * Add replications to the MC group
     */
    bcm_multicast_replication_t_init(&replication);
    replication.port = port;
    replication.encap1 = encapl_id;
    rv = bcm_multicast_add(unit, failover_mc_group, BCM_MULTICAST_INGRESS_GROUP, 1, replication);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_multicast_add\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Main flow for VPLS and MPLS Ingress 1+1 protection.
 */
int
vswitch_vpls_mpls_1plus1_protection_main(
    int unit,
    int ac_port_in,
    int pwe_port_1,
    int pwe_port_2)
{
    int rv = BCM_E_NONE;
    int ac_idx, pwe_idx, i;

    if (!vswitch_vpls_params_initialized)
    {
        /*
         * Init parameters
         */
        vswitch_vpls_params_init(unit, &vswitch_vpls_params, ac_port_in, 0, pwe_port_1, pwe_port_2);
    }
    /*
     * Create VPN
     */
    rv = vswitch_vpls_vpn_create(unit, vswitch_vpls_params.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_vpn_create\n");
        return rv;
    }

    /*
     * Create AC
     */
    l2_port_properties_s *ac_port = &vswitch_vpls_params.ac_ports[0];
    ac_port->intf = 100;
    rv = vswitch_vpls_ac_create(unit, ac_port, vswitch_vpls_params.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_ac_create %d\n", rv.ac_idx);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vswitch_vpls_params.pwe_params[1].pwe_port.intf, vswitch_vpls_params.pwe_params[1].pwe_port.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_gport_add\n", rv);
        return rv;
    }

    /*
     * Add AC to VPN
     */
    rv = vswitch_vpls_vpn_vlan_port_add(unit, vswitch_vpls_params.vpn, ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_vpn_vlan_port_add %d\n", rv, ac_idx);
        return rv;
    }

    /*
     * Create failover
     */
    rv = bcm_failover_create(unit, (BCM_FAILOVER_INGRESS | BCM_FAILOVER_WITH_ID), &failover_id_vpls);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_failover_create\n", rv);
        return rv;
    }

    /*
     * Create failover
     */
    rv = bcm_failover_create(unit, (BCM_FAILOVER_INGRESS | BCM_FAILOVER_WITH_ID), &failover_id_mpls);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_failover_create\n", rv);
        return rv;
    }

    /*
     * Create failover MC group
     */
    rv = bcm_multicast_create(unit, (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID), &failover_mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_multicast_create\n", rv);
        return rv;
    }

    /*
     * Create Protecting PWE/MPLS
     */
    vswitch_vpls_params.pwe_params[0].pwe_term.failover_id = failover_id_vpls;
    vswitch_vpls_params.pwe_params[0].pwe_term.failover_port_id = 0;
    vswitch_vpls_params.pwe_params[0].pwe_term.failover_mc_group = failover_mc_group;
    vswitch_vpls_params.pwe_params[0].pwe_term.egress_tunnel_if = 0;

    for (i = 0; i < MAX_NOF_TUNNELS; i++)
    {
        if ((vswitch_vpls_params.pwe_params[0].mpls_term_tunnel[i].label >= MIN_LABEL) && (vswitch_vpls_params.pwe_params[0].mpls_term_tunnel[i].label <= MAX_LABEL))
        {
            vswitch_vpls_params.pwe_params[0].mpls_term_tunnel[i].failover_id = failover_id_mpls;
            vswitch_vpls_params.pwe_params[0].mpls_term_tunnel[i].failover_tunnel_id = 0;
        }
    }
    vswitch_vpls_pwe_info_s *pwe_param = &vswitch_vpls_params.pwe_params[0];
    rv = vswitch_vpls_pwe_create(unit, pwe_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_pwe_create pwe_idx=%d\n", rv, 0);
        return rv;
    }

    /*
     * Add PWE to VPN
     */
    rv = vswitch_vpls_vpn_pwe_add(unit, vswitch_vpls_params.vpn, pwe_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_vpn_pwe_add pwe_idx=%d\n", rv, 0);
        return rv;
    }

    /*
     * Add PWE port to the MC group
     */
    rv = vswitch_vpls_mpls_1plus1_protection_multicast_replications_add(unit, failover_mc_group,
                                                       pwe_param->pwe_port.port, pwe_param->pwe_encap.mpls_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_mpls_1plus1_protection_multicast_replications_add pwe_idx=%d\n", rv, 0);
        return rv;
    }

    /*
     * Create Primary PWE/MPLS
     */
    vswitch_vpls_params.pwe_params[1].pwe_term.failover_id = failover_id_vpls;
    vswitch_vpls_params.pwe_params[1].pwe_term.failover_port_id = 1;
    vswitch_vpls_params.pwe_params[1].pwe_term.failover_mc_group = failover_mc_group;
    vswitch_vpls_params.pwe_params[0].pwe_term.egress_tunnel_if = 0;

    for (i = 0; i < MAX_NOF_TUNNELS; i++)
    {
        if ((vswitch_vpls_params.pwe_params[1].mpls_term_tunnel[i].label >= MIN_LABEL) && (vswitch_vpls_params.pwe_params[1].mpls_term_tunnel[i].label <= MAX_LABEL))
        {
            vswitch_vpls_params.pwe_params[1].mpls_term_tunnel[i].failover_id = failover_id_mpls;
            vswitch_vpls_params.pwe_params[1].mpls_term_tunnel[i].failover_tunnel_id = 1;
        }
    }
    *pwe_param = vswitch_vpls_params.pwe_params[1];
    rv = vswitch_vpls_pwe_create(unit, pwe_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_pwe_create pwe_idx=%d\n", rv, 1);
        return rv;
    }

    /*
     * Add PWE to VPN
     */
    rv = vswitch_vpls_vpn_pwe_add(unit, vswitch_vpls_params.vpn, pwe_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_vpn_pwe_add pwe_idx=%d\n", rv, 1);
        return rv;
    }

    /*
     * Add PWE port to the MC group
     */
    rv = vswitch_vpls_mpls_1plus1_protection_multicast_replications_add(unit, failover_mc_group,
                                                       pwe_param->pwe_port.port, pwe_param->pwe_encap.mpls_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_mpls_1plus1_protection_multicast_replications_add pwe_idx=%d\n", rv, 1);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function to change the Failover state according to user input.
 */
int
vswitch_vpls_mpls_1plus1_protection_change_failover_state(
    int unit,
    int is_mpls,
    int enabled)
{
    int rv = BCM_E_NONE;
    bcm_failover_t  failover_id;

    failover_id = ((is_mpls == TRUE) ? failover_id_mpls : failover_id_vpls);
    /*
     * Change the failover state
     */
    rv = bcm_failover_set(unit, failover_id, enabled);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_failover_set\n", rv);
        return rv;
    }

    return rv;
}
