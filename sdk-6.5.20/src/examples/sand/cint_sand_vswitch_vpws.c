/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
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
 */

/*
 * Global settings
 */

struct vswitch_vpws_pwe_info_s
{
    l2_port_properties_s pwe_port;
    mpls_tunnel_initiator_create_s mpls_encap_tunnel[MAX_NOF_TUNNELS];
    mpls_tunnel_switch_create_s mpls_term_tunnel[MAX_NOF_TUNNELS];
    mpls_port_add_s pwe_encap;
    mpls_port_add_s pwe_term;
};

struct vswitch_vpws_params_s
{
    /*
     * Flooding domain
     */
    int vpn;

    /*
     * AC ports
     */
    l2_port_properties_s ac_ports;

    /*
     * PWE ports
     */
    vswitch_vpws_pwe_info_s pwe_params;


};

/*
 * Global setup configuration struct
 */
vswitch_vpws_params_s vswitch_vpws_params;
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
    vswitch_vpws_params_s * params,
    int acP,
    int pweP)
{
    int rv = BCM_E_NONE;
    /*
     * pointers for later
     */
    vswitch_vpws_pwe_info_s *pwe_param = NULL;
    mpls_tunnel_initiator_create_s *tunnel_init = NULL;
    mpls_tunnel_switch_create_s *tunnel_term = NULL;
    mpls_port_add_s *pwe_enc = NULL;
    mpls_port_add_s *pwe_term = NULL;
    /*
     * Global parameters
     */
    params->vpn = 0x11;

    bcm_mac_t rif1_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
    bcm_mac_t mpls1_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };

     /** 1st FEC for learning **/
    if (is_device_or_above(tx_unit, JERICHO2)) {
        int fec_id;
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

    /*
     * AC parameters
     */
    params->ac_ports.port = acP;


    /*
     * PWE parameters
     */
    pwe_param = &params->pwe_params;
        /** Port **/
    pwe_param->pwe_port.intf = 100;
    pwe_param->pwe_port.mac_addr = rif1_mac;
    pwe_param->pwe_port.next_hop = mpls1_next_hop_mac;
    pwe_param->pwe_port.port = pweP;
    pwe_param->pwe_port.arp = 9001;

        /** Tunnel initiator **/
    mpls_tunnel_initiator_create_s_init(pwe_param->mpls_encap_tunnel);
    tunnel_init = &pwe_param->mpls_encap_tunnel[0];
    tunnel_init->label[0] = 0x100;
    tunnel_init->num_labels = 1;
    tunnel_init->encap_access = bcmEncapAccessTunnel2;
    tunnel_init->flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT |BCM_MPLS_EGRESS_LABEL_WITH_ID;
    if (!is_device_or_above(rx_unit, JERICHO2)) {
        tunnel_init->action = BCM_MPLS_EGRESS_ACTION_PUSH;
        tunnel_init->flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    }


    BCM_L3_ITF_SET(tunnel_init->tunnel_id, BCM_L3_ITF_TYPE_LIF, 0x3001);

        /** Tunnel termination **/
    mpls_tunnel_switch_create_s_init(pwe_param->mpls_term_tunnel);
    tunnel_term = &pwe_param->mpls_term_tunnel[0];
    tunnel_term->action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_term->label = tunnel_init->label[0];

        /** PWE encapsulation **/
    pwe_enc = &pwe_param->pwe_encap;
    mpls_port_add_s_init(pwe_encap);
    pwe_enc->flags |=  BCM_MPLS_PORT_EGRESS_TUNNEL |BCM_MPLS_PORT_ENCAP_WITH_ID;
    if (has_cw)
    {
        pwe_enc->flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    pwe_enc->flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    pwe_enc->label = 0x20000;
    pwe_enc->label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    pwe_enc->encap_access = bcmEncapAccessTunnel1;
    pwe_enc->encap_id = 8888;

        /** PWE termination **/
    pwe_term = &pwe_param->pwe_term;
    mpls_port_add_s_init(pwe_term);
    pwe_term->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pwe_term->flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    if (has_cw)
    {
        pwe_term->flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    pwe_term->flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY |
            (is_device_or_above(rx_unit, JERICHO2) ? BCM_MPLS_PORT2_CROSS_CONNECT : 0);
    pwe_term->match_label = 0x10000;
    pwe_term->encap_id = pwe_enc->encap_id;

    /** 2nd FEC for tunnel **/
    if (is_device_or_above(tx_unit, JERICHO2)) {
       int fec_id;
       rv = get_first_fec_in_range_which_not_in_ecmp_range(tx_unit, 1, &fec_id);
       if (rv != BCM_E_NONE)
       {
           printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
           return rv;
       }
       BCM_L3_ITF_SET(pwe_term->egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, fec_id);
    } else {
       BCM_L3_ITF_SET(pwe_term->egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, 0x1000);
    }

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
        rv = vswitch_vpws_params_init(tx_unit, rx_unit, &vswitch_vpws_params, acP, pweP);
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
     * Step1, create AC
     */
    vswitch_vpws_params.ac_ports.intf = ac_intf;
    l2_port_properties_s *ac_ports = &vswitch_vpws_params.ac_ports;
    rv = vswitch_vpws_ac_create(tx_unit, ac_ports, vswitch_vpws_params.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpws_ac_create %d\n", rv);
        return rv;
    }

    /*
     * Step2, create PWE
     */
    vswitch_vpws_pwe_info_s *pwe_param = &vswitch_vpws_params.pwe_params;
    rv = vswitch_vpws_pwe_create(tx_unit, rx_unit, pwe_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpws_pwe_create", rv);
        return rv;
    }
    /*
     * Step3, cross connect AC to pwe
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = ac_ports->vlan_port_id;
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
     * Step4, cross connect pwe to AC
     */

    bcm_vswitch_cross_connect_t_init (gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = pwe_param->pwe_term.mpls_port_id;
    gports.port2 = ac_ports->port;
    gports.encap2 = ac_ports->encap_id;
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
 * Creats an AC port
 */
int
vswitch_vpws_ac_create(
    int unit,
    l2_port_properties_s * ac_port,
    int vid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * Init port
     */
    rv = vswitch_vpws_port_properties(unit, ac_port, 0 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties\n", rv);
        return rv;
    }
    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);
    /*
     * set port attribures, key <port-vlan>
     */
    vlan_port.port = ac_port->port;
    vlan_port.match_vlan = vid;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vid;
    vlan_port.vsi = 0;
    if (is_device_or_above(unit, JERICHO2)) {
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    } else {
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    ac_port->vlan_port_id = vlan_port.vlan_port_id;
    ac_port->encap_id = vlan_port.encap_id;
    printf("ac_port->vlan_port_id = %d\n", ac_port->vlan_port_id);

    return rv;
}

/*
 * Creates a PWE port with the carrying MPLS tunnels and next hop ARP
 * and forward information (FEC) to be learned.
 */
int
vswitch_vpws_pwe_create(
    int tx_unit,
    int rx_unit,
    vswitch_vpws_pwe_info_s * pwe_param)
{
    int rv = BCM_E_NONE;
    /*
     * Init port
     */
    rv = vswitch_vpws_port_properties(rx_unit, &pwe_param->pwe_port, 1 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties pwe_port\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(rx_unit, &pwe_param->pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_l3_interfaces\n", rv);
        return rv;
    }
    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(rx_unit, &pwe_param->pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_arp_entry\n", rv);
        return rv;
    }
    pwe_param->mpls_encap_tunnel[0].l3_intf_id = pwe_param->pwe_port.arp;
    /*
     * Configure a tunnel initiator.
     */
    rv = vpls_create_mpls_tunnel(rx_unit, pwe_param->mpls_encap_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_mpls_tunnel\n", rv);
        return rv;
    }
    pwe_param->pwe_port.tunnel_id = pwe_param->mpls_encap_tunnel[0].tunnel_id;


    /*
     * Configure fec entry
     */
    rv = vswitch_vpws_fec_create(tx_unit, pwe_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpws_fec_create\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */

    rv = vpls_create_termination_stack(rx_unit, pwe_param->mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_termination_stack\n", rv);
        return rv;
    }
    /*
     * configure PWE tunnel - egress flow
     */
    rv = vpls_mpls_port_add_encapsulation(rx_unit, &pwe_param->pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_encapsulation\n", rv);
        return rv;
    }
    printf("pwe_encap.mpls_port_id, 0x%x \n",  pwe_param->pwe_encap.mpls_port_id);
    printf("pwe_encap.encap_id, 0x%x \n",  pwe_param->pwe_encap.encap_id);
    pwe_param->pwe_term.mpls_port_id = pwe_param->pwe_encap.mpls_port_id;
    pwe_param->pwe_term.encap_id = pwe_param->pwe_encap.encap_id;
    /*
     * configure PWE tunnel - ingress flow
     */
    pwe_param->pwe_term.egress_tunnel_if = fec_id_for_cross_connect;
     rv = vpls_mpls_port_add_termination(rx_unit, &pwe_param->pwe_term);
     if (rv != BCM_E_NONE)
     {
         printf("Error(%d), in vpls_mpls_port_add_termination\n", rv);
         return rv;
     }

    return BCM_E_NONE;
}
/*
 * Creats FECs
 */

int
vswitch_vpws_fec_create(
    int unit,
    vswitch_vpws_pwe_info_s * pwe_param)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;

    /*
     * 1. FEC for tunnel,in 2nd level FEC
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);

    fec_structure.destination  = pwe_param->pwe_port.port;
    fec_structure.tunnel_gport = pwe_param->pwe_port.tunnel_id;
    fec_structure.flags        = BCM_L3_2ND_HIERARCHY;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.fec_id       = pwe_param->pwe_term.egress_tunnel_if;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    pwe_param->pwe_term.egress_tunnel_if = fec_structure.l3eg_fec_id;
    /*
     * 2. FEC for learning,point to 2nd level FEC(tunnel)
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_FORWARD_PORT_SET(fec_structure.destination,pwe_param->pwe_term.egress_tunnel_if);
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    if ((!soc_property_get(unit, "system_headers_mode", 1) && is_device_or_above(unit, JERICHO2))
        ||!is_device_or_above(unit, JERICHO2) )
    {
        fec_structure.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }

    BCM_L3_ITF_SET(fec_structure.tunnel_gport,BCM_L3_ITF_TYPE_LIF, pwe_param->pwe_term.encap_id);
    fec_structure.fec_id      = fec_id_for_cross_connect;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    return rv;
}

/**
 * Convert system port to local port
*/

int
vswitch_vpws_system_port_to_local(int unit, int sysport, int *local_port){
    int rv = BCM_E_NONE;
    int my_modid, gport_modid;
    int gport,is_qux;
    int mod_port_id;

    rv = is_qumran_ux_only(unit, &is_qux);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_qumran_ux_only\n");
        return rv;

    }

    rv = bcm_stk_sysport_gport_get(unit, sysport, &gport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_sysport_gport_get\n");
        return rv;
    }
    printf("Sysport's gport is: 0x%x\n", gport);
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    mod_port_id = BCM_GPORT_MODPORT_PORT_GET(gport);
    printf("Required modid(core) is: 0x%x\n", gport_modid);
    printf("Sysport's local port is: 0x%x\n", mod_port_id);
    *local_port = mod_port_id;

    /*ucode_port_203.BCM88270=CPU.32:core_0.103*/
    if (is_qux) {
        *local_port = mod_port_id + 100;
    }

    return rv;
}

int
vswitch_vpws_port_properties(
    int unit,
    l2_port_properties_s * l2_port,
    uint8 create_rif)
{
    int rv = BCM_E_NONE;
    int local_port;

    rv = vswitch_vpws_system_port_to_local(unit, l2_port->port, local_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, system_port_to_local, %d\n", rv);
        return rv;
    }

    /*
     * set class for both ports
     */
    rv = bcm_port_class_set(unit, local_port, bcmPortClassId, local_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", local_port);
        return rv;
    }


    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, local_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out, port=%d\n", local_port);
        return rv;
    }

    if (create_rif)
    {
        /*
         * Set In-Port to In ETh-RIF
         */
        rv = in_port_intf_set(unit, local_port, l2_port->intf);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set intf_in, port=%d\n", local_port);
            return rv;
        }
    }
    else
    {
        rv = bcm_vlan_gport_add(unit, l2_port->intf, local_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_gport_add \n");
            return rv;
        }
    }

    return rv;
}

