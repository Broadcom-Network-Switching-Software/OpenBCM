/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * File: cint_sand_vswitch_vpws.c
 * Purpose: Example of VPWS VSwitch
 */

/*
 * File: cint_sand_vswitch_vpws.c
 * Purpose: InterIOP example of P2P setup with AC port and PWE port.
 *
 * vswitch_vpws_params_init shows an example of creating 1 AC port and 1 PWE port
 *
 * +-----------------------------------------+-------------------------------------------------------------+
 * |                                         :                                                                      |
 * |                              NWK 0  :  NWK 1                                                           |
 * |                                         :                                                                       |
 * |                                         :                        Native ETH VID: 100                   |
 * |                                         :                        VC label: 0x20000                      |
 * |                                         :                        MPLS label: 0x100                      |
 * |                                         :                        VID: 30                                     |
 * |    VID: 0x11              +--------+                RIF MAC:  00:0C:00:02:00:01      |
 * |                                |                |               Next Hop: 00:00:00:00:CD:1D     |
 * |   AC 1    <-------->|                |<--------->   PWE 1                               |
 * |                                |                |                                                              |
 * |                                |    P2p       |                                                              |
 * +-----------------------------------------+-------------------------------------------------------------+
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vpws.c
 * cint ../../src/examples/sand/cint_sand_vswitch_vpws.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint
 * exit;
 *
 * cint
 * vswitch_vpws_multi_device_main(0,2,1811939528,1811939786);
 * exit;
 *
 *  Send packet from ac to pwe
 * tx 1 psrc=201 data=0x0000000101010102030405068100001108004500001400000000800026e50a0000050a000001
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00000000cd1d000c00020001810000648847001000ff200001ffffffffff0000000101010102030405068100000008004500001400000000800026e50a0000050a000001000000000000000000000000000000000000000000000000
 *
 * Send packet from pwe to ac
 * tx 1 psrc=201 data=0x000c0002000100000000cd1d81000064884700100040100001400fffffff01020304050600000001010181000022ffff
 *
 * Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 * Data: 0x01020304050600000001010181000011ffff0000000000000000000000000000
 *
 */

/*
 * Global settings
 */

/*
 * If vswitch_vpws_params_init function should not be called in vswitch_vpls_main,
 * set this flag to 1.
 */
int vswitch_vpws_params_initialized = 0;
/*
 * control word
 */
int has_cw=0;
/*
 *FEC id for AC cross connect
 */
 int fec_id_for_cross_connect;

/*
 * Setup for example application. See file header for description
 */
int
vswitch_vpws_params_init(
    int tx_unit,
    int rx_unit,
    int acP,
    int pweP)
{
    int rv = BCM_E_NONE;
    bcm_mac_t rif1_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
    bcm_mac_t mpls1_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
    int fec_id;

    /*
     * Global parameters
     */

    /** 1st FEC for learning **/
    if (is_device_or_above(tx_unit, JERICHO2)) {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(tx_unit, 0, &fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        BCM_L3_ITF_SET(fec_id_for_cross_connect, BCM_L3_ITF_TYPE_FEC, fec_id);
    } else {
       BCM_L3_ITF_SET(fec_id_for_cross_connect, BCM_L3_ITF_TYPE_FEC, 0x5000);
    }

    /** 2nd FEC for tunnel **/
    if (is_device_or_above(tx_unit, JERICHO2)) {
       rv = get_first_fec_in_range_which_not_in_ecmp_range(tx_unit, 1, &fec_id);
       if (rv != BCM_E_NONE)
       {
           printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
           return rv;
       }
       BCM_L3_ITF_SET(cint_pwe_basic_info.mpls_encap_fec_id, BCM_L3_ITF_TYPE_FEC, fec_id);
    } else {
       BCM_L3_ITF_SET(cint_pwe_basic_info.mpls_encap_fec_id, BCM_L3_ITF_TYPE_FEC, 0x1000);
    }

    cint_pwe_basic_info.vpn = 0x11;
    cint_pwe_basic_info.pwe_port = pweP;
    cint_pwe_basic_info.pwe_intf = 100;
    cint_pwe_basic_info.mpls_port_id_ingress = 18888;
    cint_pwe_basic_info.mpls_port_id_egress = 18888;
    cint_pwe_basic_info.encap_id = 18888;
    cint_pwe_basic_info.core_arp_id = 9001;
    cint_pwe_basic_info.pwe_arp_next_hop_mac = mpls1_next_hop_mac;
    cint_pwe_basic_info.mpls_tunnel_id = 0x4001;
    cint_pwe_basic_info.mpls_tunnel_label = 0x100;
    cint_pwe_basic_info.pwe_label = 0x20000;

    return rv;
}

/*
 * Main flow. See file header for description.
 */
int
vswitch_vpws_multi_device_main(
    int tx_unit,
    int rx_unit,
    int acP,
    int pweP)
{
    int rv = BCM_E_NONE;
    int ac_intf = 100;
    int fec_id;

    if (!vswitch_vpws_params_initialized)
    {
        /*
         * Init parameters
         */
        rv = vswitch_vpws_params_init(tx_unit, rx_unit, acP, pweP);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vswitch_vpws_params_init\n", rv);
            return rv;
        }
    }

    if (has_cw) {
        int control_word = 0xffffffff;
        rv = bcm_switch_control_set (rx_unit, bcmSwitchMplsPWControlWord, control_word);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_switch_control_set %d\n", rv);
            return rv;
        }
    }

    /*
     * Configure AC port
     */
    rv = pwe_basic_vswitch_add_access_port(tx_unit, acP, cint_pwe_basic_info.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_vswitch_add_access_port %d\n", rv);
        return rv;
    }

    /*
     * Configure PWE port
     */
    int local_port;
    rv = bcm_port_local_get(rx_unit, pweP, &local_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_local_get, %d\n", rv);
        return rv;
    }

    rv = pwe_basic_configure_port_properties(rx_unit, local_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_configure_port_properties\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = pwe_basic_create_l3_interfaces(rx_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = pwe_basic_create_arp_entry(rx_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = pwe_basic_create_mpls_tunnel(rx_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    rv = pwe_basic_create_termination_stack(rx_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_termination_stack\n", rv);
        return rv;
    }


    /*
     * In non jericho 2, ARP entry configuration is done with bcm_l3_egress_create and a tunnel id of the MPLS entry.
     * It connects the MPLS entry to the ARP, Whereas in Jericho 2, bcm_mpls_tunnel_initiator_create is used to connect the MPLS to the given arp id.
     * Therefore, we need to update the ARP entry with correct MPLS tunnel.
     */
    if (!is_device_or_above(rx_unit, JERICHO2))
    {
        rv = pwe_basic_update_arp_entry(rx_unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in pwe_basic_update_arp_entry\n", rv);
            return rv;
        }
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = pwe_basic_mpls_port_add_encapsulation(rx_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - ingress flow vpn is set to 0 - so P2P is created.
     */
    rv = pwe_basic_mpls_port_add_termination(rx_unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_mpls_port_add_termination\n", rv);
        return rv;
    }

    /*
     * Configure fec entry
     */
    cint_pwe_basic_info.pwe_port = pweP;
    rv = vswitch_vpws_fec_create(tx_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpws_fec_create\n", rv);
        return rv;
    }

    /*
     * Cross connect AC to pwe
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = cint_pwe_basic_info.vlan_port_id;
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gports.port2, fec_id_for_cross_connect);
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(tx_unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), bcm_vswitch_cross_connect_add", rv);
        return rv;
    }

    /*
     * Cross connect pwe to AC
     */
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = cint_pwe_basic_info.mpls_port_id_ingress;
    gports.port2 = acP;
    gports.encap2 = cint_pwe_basic_info.ac_encap_id;
    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(rx_unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), bcm_vswitch_cross_connect_add", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Creats FECs
 */

int
vswitch_vpws_fec_create(
    int unit)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;

    /*
     * 1. FEC for tunnel,in 2nd level FEC
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);

    fec_structure.destination  = cint_pwe_basic_info.pwe_port;
    fec_structure.tunnel_gport = cint_pwe_basic_info.mpls_tunnel_id;
    fec_structure.flags        = BCM_L3_2ND_HIERARCHY;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.fec_id       = cint_pwe_basic_info.mpls_encap_fec_id;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_pwe_basic_info.mpls_encap_fec_id = fec_structure.l3eg_fec_id;

    /*
     * 2. FEC for learning,point to 2nd level FEC(tunnel)
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_FORWARD_PORT_SET(fec_structure.destination, cint_pwe_basic_info.mpls_encap_fec_id);
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    if ((!soc_property_get(unit, "system_headers_mode", 1) && is_device_or_above(unit, JERICHO2))
        ||!is_device_or_above(unit, JERICHO2) )
    {
        fec_structure.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }

    BCM_L3_ITF_SET(fec_structure.tunnel_gport, BCM_L3_ITF_TYPE_LIF, cint_pwe_basic_info.encap_id);
    fec_structure.fec_id      = fec_id_for_cross_connect;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    return rv;
}
