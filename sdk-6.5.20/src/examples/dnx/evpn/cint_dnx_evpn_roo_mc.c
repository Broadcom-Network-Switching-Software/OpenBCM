/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../../../src/examples/sand/cint_vpls_roo_mc.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/sand/cint_dnx_evpn_roo_mc.c
 *
 * cint
 * print evpn_roo_mc_main(0, 200, 201, 202, 203);
 *
 *
 *  *                              Network topology
 *
 *
 *           Access                                    Core
 *
 * Ports    access_port_1(200)                       core_port_1(202)
 *
 *          access_port_2(201)                       core_port_2(203)
 *
 *
 *                      Access    ============ >  Tunnel flow
 *
 *
 *                             +------------------+
 *                             |                  |
 *                             |                  |
 *       access_port_1 (200)   |   DA -IPMC comp  |
 *          ------------------>+                  |
 *       access_eth_rif (40)   |   DIP lookup ----+----------->IPMC Group (6000)
 *                             |                  |
 *                             |                  |
 *                             |                  |
 *                             +------------------+
 *                                                         +--------------------------------------------+
 *                                                         |                Network copy 1              |
 *                                                         |CUD : core_port_1 + core_native_eth_rif (60)|
 *                                                         |         Routed pkt - Routing modfications  |
 *                                                         |                    - Happen on native hdr  |
 *                       +-------------------------------->+   Tunnel encap :                           |
 *                       |                                 |        MPLS_LABEL -0x4444                  |
 *                       |                                 |        IML LABEL  -0x6666                  |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Network copy 2              |
 *                       |                                 |CUD : core_port_2 + core_native_eth_rif (61)|
 *                       |                                 |         Routed pkt - Routing modfications  |
 *                       |                                 |                    - Happen on native hdr  |
 *                       +-------------------------------->+   Tunnel encap :                           |
 *                       |                                 |        MPLS_LABEL -0x4445                  |
 *                       |                                 |        IML LABEL  -0x6665                  |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Network copy 3              |
 *                       |                                 |   CUD : core_port_2 + access_eth_rif (40)  |
 *                       |                                 |         fallback2bridge:Native HDR unmodify|
 *   +-----------+       +-------------------------------->+   Tunnel encap :                           |
 *   | IPMC Group|       |                                 |        MPLS_LABEL -0x4447                  |
 *   |           |       |                                 |        IML LABEL  -0x6667                  |
 *   |  6000     +======>+                                 |        ESI LABEL  -0x776                   |
 *   |           |       |                                 +--------------------------------------------+
 *   +-----------+       |
 *                       |
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 1               |
 *                       |                                 |   CUD :access_port_1 + access_eth_rif (40) |
 *                       |-------------------------------->|        fallback2bridge                     |
 *                       |                                 |        srcport exclude will discard pkt    |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 2               |
 *                       |                                 |   CUD :access_port_2 + access_eth_rif (41) |
 *                       +-------------------------------->|        routed pkt:native hdr modified      |
 *                       |                                 |                                            |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 3               |
 *                       |                                 |   CUD :access_port_1 + access_eth_rif (42) |
 *                       +-------------------------------->|       routed pkt:native hdr modified       |
 *                       |                                 |                                            |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 4               |
 *                       |                                 |   CUD :access_port_2 + access_eth_rif (40) |
 *                       +-------------------------------->|     fallbak2bridge : native hdr unmodified |
 *                                                         |                                            |
 *                                                         |                                            |
 *                                                         +--------------------------------------------+
 *
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 *                              Tunnel  ===================> Access flow
 *
 *   TX Pkt : IPv4oETH0oMPLSoMPLSoETH0
 *            Outer ETH - UC
 *            MPLS Label - 0x4445
 *            IML label -0x6667
 *            Inner ETH - Compatible MC
 *
 *                        +-------------------+              +-------------------+
 *                        |                   |              |                   |
 *                        | VT1 : OETH Term   |              | Inner ETH :       |
 *          core_port_2   | MPLS_LABEL -POP   |              |  Compatible MC    |
 *            ----------->|                   |------------->|                   |--------->IPMC Group 2
 *             txpkt      | IML_LABEL =>      |              |   DIP Lookup      |            (7000)
 *                        |(native_eth_rif +3)|              |                   |
 *                        |                   |              |                   |
 *                        +-------------------+              +-------------------+
 *
 *                                                         +--------------------------------------------+
 *                                                         |                Access copy 1               |
 *                                                         |   CUD : Access_Port_1 + access_eth_rif (40)|
 *                                                         |         Routed pkt - Routing modfications  |
 *                       +-------------------------------->+   Tunnel decap :                           |
 *                       |                                 |        MPLS_LABEL -0x4445 - POP            |
 *                       |                                 |        IML LABEL  -0x6667 ->               |
 *                       |                                 |                    (core_native_eth_rif +3)|
 *                       |                                 +--------------------------------------------+
 *                       +
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 2               |
 *                       |                                 |   CUD : Access_Port_2 + access_eth_rif (41)|
 *                       |                                 |         Routed pkt - Routing modfications  |
 *                       +-------------------------------->+   Tunnel decap :                           |
 *                       |                                 |        MPLS_LABEL -0x4445 - POP            |
 *                       |                                 |        IML LABEL  -0x6667 ->               |
 *                       |                                 |                    (core_native_eth_rif +3)|
 *                       |                                 +--------------------------------------------+
 *   +-----------+       +
 *   | IPMC Group|       |
 *   |           |       |
 *   |  7000     +======>|                                 +--------------------------------------------+
 *   |           |       +                                 |                Access copy 3               |
 *   |           |       |                                 |   CUD : Access_Port_2+core_native_eth_rif+3|
 *   |           |       |-------------------------------->+   Tunnel decap :                           |
 *   +-----------+       |                                 |        MPLS_LABEL -0x4445 - POP            |
 *                       |                                 |        IML LABEL  -0x6667 ->               |
 *                       +                                 |                    (core_native_eth_rif +3)|
 *                       +                                 |  intf match fall back2bridge               |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       +
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Network copy 1              |
 *                       |                                 |   CUD : core_Port_1+core_native_eth_rif+1  |
 *                       +                                 +   Tunnel decap :                           |
 *                       |                                 |        MPLS_LABEL -0x4445 - POP            |
 *                       +-------------------------------->|        IML LABEL  -0x6667 ->               |
 *                                                         |                    (core_native_eth_rif +3)|
 *                                                         |                                            |
 *                                                         |  Split horizon  check will happen          |
 *                                                         |  pkt gets dropped                          |
 *                                                          +--------------------------------------------+
 *
 * Note :
 *      Mostly MAC address are reused and incremented the same mac id for different copies
 *
 * Packets description
 *
 * IPoETH packet -> MC group
 * tx 1 psrc=200 data=0x01005e48c80200000000dd2f8100002808004500003500000000800012fd7fffff01e0c8c802000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0xabcdef000000abcdef008100003288470444407f0666617f01005e48c8020000fedcba008100003c080045000035000000007f0013fd7fffff01e0c8c802000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Data: 0xabcdef010000abcdef018100003388470444507f0666717f01005e48c8020000fedcba018100003d080045000035000000007f0013fd7fffff01e0c8c802000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Data: 0xabcdef020000abcdef028100003488470444607f0666807f00fff10101005e48c80200000000dd2f8100002808004500003500000000800012fd7fffff01e0c8c802000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Data: 0x5e48c80200000000dd2f8100002808004500003500000000800012fd7fffff01e0c8c802000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Data: 0x5e48c8020000000012368100002a080045000035000000007f0013fd7fffff01e0c8c802000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Data: 0x5e48c80200000000123581000029080045000035000000007f0013fd7fffff01e0c8c802000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 *
 * MPLS over ethernet packet -> MC group
 * tx 1 psrc=203 data=0x0000abcdef0100ffabcdef018847044450800666718001005e48c80400000000fd2f08004500003500000000800012f97fffff03e0c8c804000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x5e48c80400000000123481000028080045000035000000007f0013f97fffff03e0c8c804000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Data: 0x5e48c80400000000123581000029080045000035000000007f0013f97fffff03e0c8c804000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 * MPLS over ethernet packet
 * tx 1 psrc=203 data=0x0000abcdef0100ffabcdef01884704445080066670801232118001005e48c80400000000fd2f08004500003500000000800012f97fffff03e0c8c804000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x5e48c80400000000123481000028080045000035000000007f0013f97fffff03e0c8c804000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Data: 0x5e48c80400000000123581000029080045000035000000007f0013f97fffff03e0c8c804000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 */

int IML_DUAL_HOME_PEER_PROFILE = 0x123;
int IML_DUAL_HOME_PEER_ESI_LABEL = 0xfff;

/* Main function having required calls*/
int
evpn_roo_mc_main(
    int unit,
    int access_port_1,
    int access_port_2,
    int core_port_1,
    int core_port_2)
{
    int rv = BCM_E_NONE;
    int i;

    /*
     * Ports assignment
     */
    vpls_roo_mc_init(access_port_1, access_port_2, core_port_1, core_port_2);

    /*
     * Set port properties like create vlan domain and create in and out lifs
     */
    rv = vpls_roo_mc_configure_port_properties(unit, &cint_vpls_roo_mc_info.access_port,
                                               &cint_vpls_roo_mc_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error %d, in vpls_roo_mc_configure_port_properties\n", rv);
        return rv;
    }

    /*
     * create vsi
     */
    rv = vpls_roo_mc_create_eth_rifs(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_eth_rifs\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_roo_mc_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * arpid to mac
     */
    core_arp_flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    rv = vpls_roo_mc_create_arp_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_arp_entries\n", rv);
        return rv;
    }

    for (i = 0; i < NUM_OF_CORE_PORTS; i++)
    {
        /*
         * Configure a push mpls tunnel entry.
         */
        rv = vpls_roo_mc_create_mpls_tunnel(unit,
                                            cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + i,
                                            cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + i,
                                            1, cint_vpls_roo_mc_info.core_overlay_arp_id[i],
                                            &cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[i]);
        printf("create_mpls_tunnel : label_1 =%x, label_2=%x, overlay_arp_id=%d, mpls_tunnel_id=%d\n",
               cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + i,
               cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + i, cint_vpls_roo_mc_info.core_overlay_arp_id[i],
               cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_roo_mc_create_mpls_tunnel\n", rv);
            return rv;
        }
    }

    /*
     * core fall back to switch copy
     */
    rv = vpls_roo_mc_create_mpls_tunnel(unit,
                                        cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + 2,
                                        cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + 2,
                                        1, cint_vpls_roo_mc_info.core_overlay_arp_id[2],
                                        &cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[2]);
    printf("create_mpls_tunnel : label_1 =%x, label_2=%x, overlay_arp_id=%d, mpls_tunnel_id=%d\n",
           cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + 2,
           cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + 2, cint_vpls_roo_mc_info.core_overlay_arp_id[2],
           cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * configure IML label
     */

    rv = evpn_roo_mc_iml_add_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /*
     * insert LEM and KAPS entries for fwd lookup
     */
    rv = vpls_roo_mc_fwd_lookup_entires_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_fwd_lookup_entires_create\n", rv);
        return rv;
    }

    /*
     * create IPMC group for native to tunnel flow
     */
    rv = vpls_roo_mc_create_ipmc_group1(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_ipmc_group1\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    bcm_mpls_label_t label_array[2];
    label_array[0] = cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + 1;
    label_array[1] = cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + 1;
    rv = vpls_roo_mc_create_termination_stack(unit, label_array, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_termination_stack\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = evpn_roo_mc_iml_add_termination(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_mpls_port_add_termination\n", rv);
        return rv;
    }

    /*
     * create IPMC group tunnel to native flow
     */
    rv = vpls_roo_mc_create_ipmc_group2(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_ipmc_group2\n", rv);
        return rv;
    }

    return rv;
}


/*
 * vpls_roo_mc_mpls_port_add_encapsulation
 */
int
evpn_roo_mc_iml_add_encapsulation(
    int unit)
{
    int rv = BCM_E_NONE;

    bcm_mpls_egress_label_t iml_enc;
    bcm_mpls_egress_label_t *plabel = &iml_enc;
    int label_i;
    int lif;

    bcm_mpls_egress_label_t_init(plabel);
    plabel->flags = BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;

    for (label_i = 0; label_i < NUM_OF_CORE_PORTS + 1; label_i++)
    {
        bcm_gport_t gport;
        plabel->label = cint_vpls_roo_mc_info.core_overlay_pwe_label + label_i;
        rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_mpls_tunnel_initiator_create iml %d\n", label_i);
            return rv;
        }
        lif = BCM_L3_ITF_VAL_GET(plabel->tunnel_id);
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, plabel->tunnel_id);
        printf("IML encapsulation added: label = 0x%x, lif = 0x%x\n", plabel->label, lif);

        cint_vpls_roo_mc_info.core_overlay_mpls_port_id_eg[label_i] = gport;
        cint_vpls_roo_mc_info.core_overlay_pwe_encap_id[label_i] = lif;

        rv = bcm_port_class_set(unit, gport, bcmPortClassEgress, IML_DUAL_HOME_PEER_PROFILE);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set for dual homed peer id (%d) of lif (0x%08x)\n",
                   IML_DUAL_HOME_PEER_PROFILE, lif);
            return rv;
        }
    }

    /*
     * Add ESI enapsulation (should be ignored)
     */
    rv = evpn_roo_mc_esi_encaps_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: evpn_roo_mc_esi_encaps_add\n");
        return rv;
    }

    return rv;
}

int
evpn_roo_mc_esi_encaps_add(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_esi_info_t esi_info;
    int port_i;

    bcm_mpls_esi_info_t_init(&esi_info);
    esi_info.esi_label = IML_DUAL_HOME_PEER_ESI_LABEL;
    esi_info.out_class_id = IML_DUAL_HOME_PEER_PROFILE;

    for (port_i = 0; port_i < NUM_OF_ACCESS_PORTS; port_i++)
    {
        esi_info.src_port = cint_vpls_roo_mc_info.access_port[port_i];
        rv = bcm_mpls_esi_encap_add(unit, &esi_info);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_mpls_esi_encap_add port=%d\n", esi_info.src_port);
            return rv;
        }
    }

    return rv;
}

int
evpn_roo_mc_iml_add_termination(
    int unit)
{
    int rv = BCM_E_NONE;
    int vpn = cint_vpls_roo_mc_info.core_native_eth_rif + 3;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_mpls_tunnel_switch_t *pswitch = &tunnel_switch;
    bcm_mpls_range_action_t action;
    int feature_mpls_term_1_or_2_labels = *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL);

    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    rv = bcm_mpls_range_action_add(unit,cint_vpls_roo_mc_info.core_overlay_pwe_label,
                                   cint_vpls_roo_mc_info.core_overlay_pwe_label+2, &action);
    if (rv != BCM_E_NONE) {
        printf("ERROR: in bcm_mpls_range_action_add\n", proc_name);
        return rv;
    }

    bcm_mpls_tunnel_switch_t_init(pswitch);
    pswitch->action = BCM_MPLS_SWITCH_ACTION_POP;
    pswitch->flags = BCM_MPLS_SWITCH_EVPN_IML;
    pswitch->flags |= (feature_mpls_term_1_or_2_labels ? 0 : BCM_MPLS_SWITCH_EXPECT_BOS);
    pswitch->label = cint_vpls_roo_mc_info.core_overlay_pwe_label + 1;
    pswitch->vpn = vpn;
    rv = bcm_mpls_tunnel_switch_create(unit, pswitch);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_switch_create iml label\n");
        return rv;
    }
    printf("IML Termination added. gport = 0x%x", pswitch->tunnel_id);

    if (!feature_mpls_term_1_or_2_labels) {
        /**If the feature term 1 or 2 mpls labels is available, no need to add the 2nd IML term entry.*/
        pswitch->flags = BCM_MPLS_SWITCH_EVPN_IML;
        rv = bcm_mpls_tunnel_switch_create(unit, pswitch);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_mpls_tunnel_switch_create iml label\n");
            return rv;
        }
        printf("IML+ESI Termination added. gport = 0x%x", pswitch->tunnel_id);
    }

    return rv;

}
