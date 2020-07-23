/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_mpls_deep_stack.c Purpose: utility for MPLS deep encapsulation stack.. 
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
 * cint ../../../../src/examples/dnx/mpls/cint_mpls_deep_stack.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * int nof_tunnel = 6;
 * rv = mpls_deep_stack_example(unit,in_port,out_port,nof_tunnel,0);
 * print rv; 
 * 
 * For vpls over deep stack service:
 * rv = mpls_deep_stack_vpls_example(unit,in_port,out_port,nof_tunnel);
 * 
 
 *  Scenarios configured in this cint:
 *  In each cint itteration only 1 scenario is configured, based on defined 'mode'
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   mode = 0 - basic with 1 MPLS label
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an 1 MPLS and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic from port=200
 *   ----------------------------->port=200
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+--+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA                |     SA                || ethtype  |        IPv4
 *   |    |00:0c:00:02:00:00       |00:00:07:00:01:00      || 0x800    | 0x4500
 *   |    |                        |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS     ||  MPLS    ||  .......... ||  MPLS       ||    IPv4  
 *   |    |0c:00:02:00:01        |00:00:00:cd:1d        ||Label:0x6100  ||Label:0x6000||Label:5100||  .......... ||0x1000       ||    0x4500
 *   |    |                      |                       ||Exp:0         ||Exp:0      ||               ......... ||             ||                      ||
 *   |    |                      |                      ||TTL:20          ||TTL:20    ||             ............||             ||                      ||
 *   |    |                      |                      ||<---------------------------------nof_tunnel*2----------------------->||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

int
mpls_deep_stack_ipv6_example(
    int unit,
    int in_port,
    int out_port,
    int nof_tunnel,
    int start_phase)
{
    int rv = BCM_E_NONE;
    int encap_access_array[8] = {
                                        bcmEncapAccessRif,
                                        bcmEncapAccessNativeArp,
                                        bcmEncapAccessTunnel1,
                                        bcmEncapAccessTunnel2,
                                        bcmEncapAccessTunnel3,
                                        bcmEncapAccessTunnel4,
                                        bcmEncapAccessArp,
                                        bcmEncapAccessInvalid
                                    };
    int encap_access_idx = 0;
    int mpls_tunnel_id = 0;
    bcm_mpls_label_t stage_enc_label_1 = 0x3331;
    bcm_mpls_label_t stage_enc_label_2 = 0x4441;

    MPLS_UTIL_MAX_NOF_ENTITIES = 2;

    init_default_mpls_params(unit, in_port, out_port);

    mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
    
    /* encapsulation entries */
    for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
    {
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx+start_phase];
    }
    mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &mpls_util_entity[1].arps[0].arp;

    /* configure ARP+AC in single ETPS entry */
    mpls_util_entity[1].arps[0].flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    rv = mpls_util_main(unit,in_port,out_port);
    if (rv) {
        printf("Error, create_mpls_tunnel\n");
        return rv;
    }

    return rv;
}

int
mpls_deep_stack_vpls_example(
    int unit,
    int in_port,
    int out_port,
    int nof_tunnel)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_mpls_port_t mpls_port;
    bcm_vswitch_cross_connect_t gports;
    int start_phase;
    bcm_encap_access_t vpls_encap_access;

    /* In case of nof_tunnel is 5 shift MPLS phases by 1 so that VPLS will start at phase bcmEncapAccessNativeArp */
    if (nof_tunnel == 5)
    {
        start_phase = 2;
        vpls_encap_access = bcmEncapAccessNativeArp;
    }
    else
    {
        start_phase = 1;
        vpls_encap_access = bcmEncapAccessRif;
    }

    rv = mpls_deep_stack_ipv6_example(unit,in_port,out_port,nof_tunnel,start_phase);
    if (rv) {
        printf("Error, create_mpls_tunnel\n");
        return rv;
    }


    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = in_port;
    vlan_port.match_vlan = 20;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, 20, in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |=  BCM_MPLS_PORT_EGRESS_TUNNEL;

    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;


    mpls_port.egress_label.label = 0x3333;
    mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_port.egress_label.encap_access = vpls_encap_access;
    mpls_port.egress_tunnel_if = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
        return rv;
    }

    bcm_vswitch_cross_connect_t_init(&gports);

    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.encap1 = BCM_FORWARD_ENCAP_ID_INVALID;

    printf("Cross Connecting AC to PWE\n");
    gports.port1 = vlan_port.vlan_port_id;
    gports.encap2 = mpls_port.encap_id;
    gports.port2 = out_port;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_vswitch_cross_connect_add AC(0x%08X) to PWE(0x%08X)\n",
               vlan_port.vlan_port_id, mpls_util_entity[0].fecs[0].fec_id);
        return rv;
    }


    return rv;
}

int
mpls_deep_stack_lsr_example(
    int unit,
    int in_port,
    int out_port,
    int nof_tunnel,
    int start_phase)
{
    int rv = BCM_E_NONE;
    int encap_access_array[8] = {
                                        bcmEncapAccessRif,
                                        bcmEncapAccessNativeArp,
                                        bcmEncapAccessTunnel1,
                                        bcmEncapAccessTunnel2,
                                        bcmEncapAccessTunnel3,
                                        bcmEncapAccessTunnel4,
                                        bcmEncapAccessArp,
                                        bcmEncapAccessInvalid
                                    };
    int encap_access_idx = 0;
    int mpls_tunnel_id = 0;
    bcm_mpls_label_t stage_enc_label_1 = 0x3331;
    bcm_mpls_label_t stage_enc_label_2 = 0x4441;

    MPLS_UTIL_MAX_NOF_ENTITIES = 2;

    init_default_mpls_params(unit, in_port, out_port);

    mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
    
    /* encapsulation entries */
    for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
    {
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx+start_phase];
    }
    mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &mpls_util_entity[1].arps[0].arp;


    mpls_util_entity[0].mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;

    /* configure ARP+AC in single ETPS entry */
    mpls_util_entity[1].arps[0].flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    rv = mpls_util_main(unit,in_port,out_port);
    if (rv) {
        printf("Error, create_mpls_tunnel\n");
        return rv;
    }

    return rv;
}


