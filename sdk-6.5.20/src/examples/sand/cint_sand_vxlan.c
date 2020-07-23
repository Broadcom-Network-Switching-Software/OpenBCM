/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Test Scenario: flooding, UC packets
 *
 * soc properties:
 * custom_feature_egress_independent_default_mode=1
 * bcm886xx_ip4_tunnel_termination_mode=0
 * bcm886xx_l2gre_enable=0
 * bcm886xx_vxlan_enable=1
 * bcm886xx_ether_ip_enable=0
 * bcm886xx_vxlan_tunnel_lookup_mode=2
 * bcm886xx_auxiliary_table_mode=1 (for arad/arad+ device only)
 * split_horizon_forwarding_groups_mode=1 (For Jericho device and forward)
 * Example of basic VXLAN configuration
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_ip_tunnel.c
 * cint ../../src/examples/sand/cint_sand_field_fec_id_align_wa.c
 * cint ../../src/examples/sand/cint_sand_vxlan.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int access_port = 200;
 * int provider_port = 203;
 * int vdc_enable = 0;
 * rv = vxlan_example(unit, access_port, provider_port, vdc_enable);
 * print rv;
 *
 * Traffic:
 * vxlan- unknown destination- packet from vxlan tunnel- flood
 * Purpose: - check VXLAN tunnel termination
 *          - check split horizon filter
 *          - learn native SA
 * Packet flow:
 * - tunnel is terminated
 * - get VSI from VNI
 * - learn reverse FEC for native SA
 * - packet is bridged: forwarded according to DA and VSI
 * - no hit, do flooding
 * - get flooding MC group from VSI
 * - for MC group entry to access: AC outlif and port
 * - for MC group entry to core: tunnel outlif and port
 * -                             entry is filtered at split horizon filter at egress
 *
 * tx 1 psrc=201 data=0x000c0002000000000700010081000014080045000057000000008011d955a00f1011a10f1011555512b5004300000800000000138800000000000100000000000581810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20

 * Received packets on unit 0 should be:
 * 0x000000000100000000000581810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 *  vxlan- unknown destination- packet from user - flood
 *  Purpose: - check VXLAN tunnel encapsulation
 * Packet flow:
 * - get VSI from port default VSI
 * - learn AC for SA
 * - packet is bridged: forwarded according to DA and VSI
 * - no hit, do flooding
 * - get flooding MC group from VSI
 * - for MC group entry to access: AC outlif and port
 * - for MC group entry to core: tunnel outlif and port
 *   - vxlan tunnel is built using IP tunnel eedb entry.
 *   - vxlan header.vni is resovled from VSI -> vni
 *   - ethernet header is built using LL eedb entry (IP tunnel eedb entry point to LL eedb entry):
 *   - DA from LL eedb entry, SA from LL eedb entry.VSI.myMac
 *   - VLAN = entry.VSI
 *
 * tx 1 psrc=200 data=0000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * packet1:
 *  0x0000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * packet2:
 *  0x00000000cd1d000c0002000081000064080045281801000000003c110584a00f1011a10f10115000555517ed000008000000001388000000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * vxlan- known learned destination- packet from user -
 * Purpose: - send packet using learned tunnel from previous flow
 * Packet flow:
 * - get VSI from port default VSI
 * - packet is bridged: forwarded according to DA and VSI
 * - hit, result is FEC: tunnel outlif + port
 *   - vxlan tunnel is built using IP tunnel eedb entry.
 *   - vxlan header.vni is resovled from VSI -> vni
 *   - ethernet header is built using LL eedb entry (IP tunnel eedb entry point to LL eedb entry):
 *   - DA from LL eedb entry, SA from LL eedb entry.VSI.myMac
 *   - VLAN = entry.VSI
 *
 * tx 1 psrc=200 data=0x000000000581000012345678810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * 0x00000000cd1d000c0002000081000064080045280bfe000000003c111187a00f1011a10f1011500055550bea00000800000000138800000000000581000012345678810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * vxlan- known learned destination- packet from vxlan tunnel- flood
 * Purpose: - send packet using learned AC outlif from previous flow
 *
 * tx 1 psrc=201 data=0x000c0002000000000700010081000014080045000057000000008011d955a00f1011a10f1011555512b50043000008000000001388000000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * 0x0000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */


/*
 * Define learn mode
 */
enum cint_sand_vxlan_learn_mode_t {
    LEARN_FEC_ONLY = 0,
    LEARN_SYM_OUTLIF_PORT = 1,
    LEARN_SYM_OUTLIF_FEC = 2
};

/*
 * VXLAN global structure
 */
struct cint_vxlan_basic_info_s
{
    bcm_gport_t vxlan_port_id;              /* VXLAN port id */
    bcm_gport_t vlan_port_id;               /* VLAN port id */
    int vpn_id;                             /* VPN ID */
    int vni;                                /* VNI of the VXLAN */
    int vxlan_network_group_id;             /* ingress and egress orientation for VXLAN */
    int vxlan_vdc_enable;                   /* Option to enable/disable VXLAN VDC support */
    int vxlan_vdc;                          /* VXLAN VDC value */
    bcm_vlan_t access_vid;                  /* access vid for AC */
    bcm_mac_t access_eth_fwd_mac;           /* mac for bridge fwd */
    bcm_mac_t provider_eth_fwd_mac;         /* mac for bridge fwd */
    int vxlan_egress_if_is_ecmp;            /* Indicates whether the VXLAN egress if is ECMP */
    int add_routing_table_entry;            /* feature: add entry to routing table: available for Jericho and above */
    int first_level_ecmp_enable;            /* use ECMP in first level FEC */
    int second_level_ecmp_enable;           /* use ECMP in second level FEC */
    int use_slb_hashing_for_ecmp;           /* use slb hashing for ecmp (vs. configured ECMP hashing)*/
    int native_vlan_action_id;              /* vlan action id */
    cint_sand_vxlan_learn_mode_t learn_mode;/* learn mode */
};


cint_vxlan_basic_info_s cint_vxlan_basic_info =
{
    /* vxlan_port_id */
    0,
    /* vlan_port_id */
    0,
    /* vpn_id */
    15,
    /* vni */
    5000,
    /* VXLAN vxlan_network_group_id */
    1,
    /* VXLAN vxlan_vdc_enable */
    0,
    /* VXLAN vxlan_vdc */
    0,
    /* access_vid */
    510,
    /* access_eth_fwd_mac */
    {0x00, 0x00, 0xA5, 0x5A, 0xA5, 0x5A},
    /* provider_eth_fwd_mac */
    {0x00, 0x00, 0xA6, 0x6A, 0xA6, 0x6A},
    /* vxlan_egress_if_is_ecmp */
    0,
    /* add_routing_table_entry */
    0,
    /*first_level_ecmp_enable */
    0,
    /*second_level_ecmp_enable */
    0,
    /* use_slb_hashing_for_ecmp */
    0,
    /* learn mode */
    LEARN_FEC_ONLY,
    /* JR2 only: esem name space arr prefix */
    0
};

void
vxlan_basic_init(
    int unit,
    int access_port,
    int provider_port,
    int vdc_enable)
{
    int rv = BCM_E_NONE;

    ip_tunnel_basic_info.access_port = access_port;
    ip_tunnel_basic_info.provider_port = provider_port;
    ip_tunnel_basic_info.tunnel_type[0] = bcmTunnelTypeVxlan;

    /*  Get soc property about VXLAN VDC support */
    if (vdc_enable) {
        if (is_device_or_above(unit, JERICHO2) || soc_property_get(unit , "bcm886xx_vxlan_vpn_lookup_mode", 0)) {
            cint_vxlan_basic_info.vxlan_vdc_enable = 1;
            cint_vxlan_basic_info.vxlan_vdc = 10;
        }
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        /* init L2 VXLAN module,  needed only for JER1*/
        rv = bcm_vxlan_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vxlan_init \n");
            return rv;
        }

        port_tpid_init(access_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with access_port\n");
            return rv;
        }

        port_tpid_init(provider_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with provider_port\n");
            return rv;
        }

        /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
               are not configured. This is here to compensate. */
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    return rv;
}

int
vxlan_network_domain_set(
    int unit,
    int access_port,
    int provider_port,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;

    if (cint_vxlan_basic_info.vxlan_vdc_enable) {
        if (is_device_or_above(unit, JERICHO2)) {
            rv = bcm_port_class_set(unit, ip_tunnel_basic_info.term_tunnel_id[tunnel_idx], bcmPortClassIngress, cint_vxlan_basic_info.vxlan_vdc);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set unit %d, port %x rv %d\n", unit, ip_tunnel_basic_info.term_tunnel_id[tunnel_idx], rv);
                return rv;
            }
            rv = bcm_port_class_set(unit, ip_tunnel_basic_info.encap_tunnel_id[tunnel_idx], bcmPortClassEgress, cint_vxlan_basic_info.vxlan_vdc);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set unit %d, port %x rv %d\n", unit, ip_tunnel_basic_info.encap_tunnel_id[tunnel_idx], rv);
                return rv;
            }
        } else {
            rv = bcm_port_class_set(unit, access_port, bcmPortClassId, cint_vxlan_basic_info.vxlan_vdc);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set unit %d, port %x rv %d\n", unit, access_port, rv);
                return rv;
            }
            rv = bcm_port_class_set(unit, provider_port, bcmPortClassId, cint_vxlan_basic_info.vxlan_vdc);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set unit %d, port %x rv %d\n", unit, provider_port, rv);
                return rv;
            }
        }
    }

    return rv;
}


/* this configure the bounce back filter to drop.
*/
int vxlan_bounce_back_filter_init(int unit) {
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int trap_id;

    bcm_rx_trap_config_t_init(&config);

    config.flags = (BCM_RX_TRAP_UPDATE_DEST);
    config.trap_strength = 7;
    config.dest_port=BCM_GPORT_BLACK_HOLE;

    rv = bcm_rx_trap_type_create(unit,flags,bcmRxTrapEgTrillBounceBack,&trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap %d \n", trap_id);
        return rv;
    }

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    return rv;
}

/* this configure the orientation on vxlan to be 2b.
   Configure the orientation filters:
   filter AC inLif    (network group: 0) to VxLAN outLif (network group: 2)
   filter VxLAN inLif (network group: 2) to AC outLif    (network group: 0)
*/
int vxlan_orientation_filter_init(int unit) {
    int rv = BCM_E_NONE;

    /* set the orientation for vxlan to be 2b: 2 */
    cint_vxlan_basic_info.vxlan_network_group_id = 2;

    /* set orientation filter: filter AC inLif (network group: 0) to VxLAN outLif (network group: 2) */
    /* set the AC network group */
    bcm_switch_network_group_t ac_network_group = 0;
    /* set the vxlan network group */
    bcm_switch_network_group_config_t network_group_config;
    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = cint_vxlan_basic_info.vxlan_network_group_id;
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE; /* drop the packet */
    /* set orientation filter*/
    rv = bcm_switch_network_group_config_set(unit,
                                        ac_network_group,
                                        &network_group_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_network_group_config_set\n");
        return rv;
    }
    printf("add orientation filter from AC (%d) to VXLAN (%d) \n", ac_network_group, cint_vxlan_basic_info.vxlan_network_group_id);

    /* set orientation filter: filter VxLAN inLif(network group : 2) to AC outLif(network group: 0) */
    /* set the AC network group */
    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = ac_network_group;  /* AC network group */
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE; /* drop the packet */
    /* set orientation filter */
    rv = bcm_switch_network_group_config_set(unit,
                                        cint_vxlan_basic_info.vxlan_network_group_id, /* vxlan network group id */
                                        &network_group_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_network_group_config_set\n");
        return rv;
    }

    printf("add orientation filter from VXLAN (%d) to AC (%d) \n", cint_vxlan_basic_info.vxlan_network_group_id, ac_network_group);

    return rv;
}

int vxlan_split_horizon_set(int unit, int vpn, int vdc, int vxlan_network_group_id)
{
    int rv;
    bcm_vlan_port_t vlan_port;

    /** j2 only: configure entry: VNI+outlif */
    if (is_device_or_above(unit,JERICHO2)) {
        /* At ESEM for vsi, netork_domain -> VNI, outlif profile
           create a virtual AC which to configure the outlif profile in ESEM entry.
         */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vsi = vpn; /* vsi, part of the key in ESEM lookup */
        vlan_port.match_class_id = vdc; /* network domain, part of the key in ESEM lookup */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.egress_network_group_id = vxlan_network_group_id;
        printf("bcm_vlan_port_create with orientation: %d \n", vlan_port.egress_network_group_id);
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vlan_port_create \n", proc_name);
            return rv;
        }
    }

    return rv;
}

/**
 * Configure orientation for vxlan tunnel outlif
 */
int vxlan_outlif_update_orientation(
    int unit,
    int tunnel_idx) {

    int rv;

    rv = bcm_port_class_set(unit,
                            ip_tunnel_basic_info.encap_tunnel_id[tunnel_idx],
                            bcmPortClassForwardEgress,
                            cint_vxlan_basic_info.vxlan_network_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set \n");
        return rv;
    }
    return rv;
}

int vxlan_vxlan_port_add(
    int unit,
    int provider_port,
    int tunnel_idx,
    int is_ecmp)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    vxlan_port_add_s vxlan_port_add;
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = cint_vxlan_basic_info.vpn_id;
    vxlan_port_add.in_port = provider_port;
    vxlan_port_add.in_tunnel = ip_tunnel_basic_info.term_tunnel_id[tunnel_idx];

    /* in case we want to learn FEC only. Don't set the tunnel outlif. */
    if (cint_vxlan_basic_info.learn_mode != LEARN_FEC_ONLY) {
        vxlan_port_add.out_tunnel = ip_tunnel_basic_info.encap_tunnel_id[tunnel_idx];
    }

    vxlan_port_add.flags = 0;

    vxlan_port_add.network_group_id = cint_vxlan_basic_info.vxlan_network_group_id;
    /* is learn is symmetric lif + FEC then learn ARP FEC */
    if (cint_vxlan_basic_info.learn_mode == LEARN_SYM_OUTLIF_FEC) {
        vxlan_port_add.egress_if = ip_tunnel_basic_info.arp_fec_id[tunnel_idx];
    }
    else if (cint_vxlan_basic_info.learn_mode == LEARN_SYM_OUTLIF_PORT) {
        /* no fec configured here*/
    } else if (cint_vxlan_basic_info.learn_mode == LEARN_FEC_ONLY) {
        if (!is_ecmp) {
            vxlan_port_add.egress_if = ip_tunnel_basic_info.encap_fec_id[tunnel_idx];
        } else {
            /* FEC passed to vxlan port is an ECMP. Therefore, we can't add directly the vxlan port to the VSI flooding MC group
            We'll add instead the tunnel initiator gport to the VSI flooding MC group */
            vxlan_port_add.is_ecmp = 1;    /* this vxlan port uses ECMP */
            vxlan_port_add.egress_if = ip_tunnel_basic_info.encap_ecmp[tunnel_idx];
        }
    }

    vxlan_port_add.out_tunnel_if = ip_tunnel_basic_info.encap_tunnel_intf[tunnel_idx];
    vxlan_port_add.add_to_mc_group = 1;
    if(verbose >= 1) {
        printf("bcm_vxlan_port_add with orientation: %d \n", vxlan_port_add.network_group_id);
    }
    rv = vxlan__vxlan_port_add(unit, vxlan_port_add);
    cint_vxlan_basic_info.vxlan_port_id = vxlan_port_add.vxlan_port_id;
    if(verbose >= 1) {
        printf("created vxlan_port_id:0x%08x \n", cint_vxlan_basic_info.vxlan_port_id);
    }

    /* Update the egress orientation for the outlif.
       In case of FEC only, vxlan port API didn't update the orientation for the outlif,
       since no outlif were passed as parameter to the vxlan_port_add API, only FEC.  */
    if (cint_vxlan_basic_info.learn_mode == LEARN_FEC_ONLY) {
        rv = bcm_port_class_set(unit, ip_tunnel_basic_info.encap_tunnel_id[tunnel_idx], bcmPortClassForwardEgress, cint_vxlan_basic_info.vxlan_network_group_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set \n");
            return rv;
        }
        if(verbose >= 1) {
            printf("update tunnel outlif 0x%x with orientation: %d \n", ip_tunnel_basic_info.encap_tunnel_id[tunnel_idx], vxlan_port_add.network_group_id);
        }
    }

    return BCM_E_NONE;
}

int vxlan_vlan_port_add(int unit, int port, bcm_vlan_t vlan, bcm_gport_t *vlan_port_id){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_action_set_t action;
    bcm_vlan_port_t_init(&vlan_port);
    uint8 out_ac_ttl_enable = 0;

	/* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add (unit, vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add \n");
        return rv;
    }

    /* the match criteria is port:1, out-vlan:510   */
    if (!is_device_or_above(unit, JERICHO2))
    {
        /* JR1 - criteria is PORT * VLAN */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;

        out_ac_ttl_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "out_ac_ttl_enable", 0);
    }
    else
    {
        /* JR2 - criteria is PORT * C_VLAN */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    }
    vlan_port.port = port;
    vlan_port.match_vlan = vlan;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vlan;
    vlan_port.flags = 0;
    vlan_port.vsi = 0;
    if (out_ac_ttl_enable)
    {
        vlan_port.flags |= BCM_VLAN_PORT_EGRESS_ARP_POINTED;
    }
    rv = bcm_vlan_port_create(unit, &vlan_port);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    if(verbose >= 2) {
        printf("Add vlan-port-id:0x%08x in-port:0x%08x match_vlan:0x%08x match_inner_vlan:0x%08x\n\r",vlan_port.vlan_port_id, vlan_port.port, vlan_port.match_vlan, vlan_port.match_inner_vlan);
    }

    /* handle of the created gport */
    *vlan_port_id = vlan_port.vlan_port_id;
    if(verbose >= 1) {
        printf("created vlan_port_id:0x%08x \n", vlan_port.vlan_port_id);
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        if (advanced_vlan_translation_mode) {
            int ing_tag_format = 2;
            int ing_vlan_action_id;
            int ing_vlan_edit_class_id = 4;
            bcm_vlan_port_translation_t port_trans;
            bcm_vlan_translate_action_class_t action_class;

            /* Map LIF -> vlan_edit_class_id */
            bcm_vlan_port_translation_t_init(&port_trans);
            port_trans.gport = vlan_port.vlan_port_id;
            port_trans.vlan_edit_class_id = ing_vlan_edit_class_id;
            port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
            rv = bcm_vlan_port_translation_set(unit, &port_trans);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_translation_set ing $rv\n");
                return rv;
            }
            /* Create ingress vlan action id: ing_vlan_action_id */
            rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS, &ing_vlan_action_id);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
                return rv;
            }
            /* Map <vlan_edit_class_id,  tag_format_class_id> -> ing_vlan_action_id */
            bcm_vlan_translate_action_class_t_init(&action_class);
            action_class.vlan_edit_class_id = ing_vlan_edit_class_id;
            action_class.tag_format_class_id = ing_tag_format;
            action_class.vlan_translation_action_id = ing_vlan_action_id;
            action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
            rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
                return rv;
            }
            /* Map ing_vlan_action_id to action */
            bcm_vlan_action_set_t_init(&action);
            action.dt_outer = bcmVlanActionNone;
            action.dt_inner = bcmVlanActionNone;
            action.outer_tpid = 0x8100;
            action.inner_tpid = 0x8100;
            rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
                return rv;
            }
        } else {
            bcm_vlan_action_set_t_init(&action);
            action.ot_outer = bcmVlanActionNone;
            action.ot_inner = bcmVlanActionNone;
            rv = bcm_vlan_translate_action_create(unit, vlan_port.vlan_port_id, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_vlan_translate_action_create $rv\n");
                return rv;
            }
        }
    }
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_vlan_gport_add(unit, vlan, port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_gport_add\n");
            return rv;
        }
    }

    return rv;
}

int vswitch_add_port(int unit, bcm_vlan_t vsi, bcm_port_t phy_port, bcm_gport_t gport){
    int rv;

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit, vsi, gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /* update Multicast to have the added port  */
    rv = multicast_vlan_port_add(unit, vsi, phy_port , gport);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_add_port_1\n");
        return rv;
    }
    if(verbose){
        printf("add port   0x%08x to multicast \n\r",gport);
    }

    return rv;
}


/*
 * add gport of type vlan-port to the multicast
 */
int multicast_vlan_port_add(int unit, int mc_group_id, int access_port, int gport){
    int rv;
    int encap_id;
    int flags;
    bcm_multicast_replication_t replications;

    rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, access_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, access_port, gport);
        return rv;
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        rv = bcm_multicast_ingress_add(unit, mc_group_id, access_port, encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, access_port, encap_id);
            return rv;
        }
    } else {
        flags = BCM_MULTICAST_INGRESS_GROUP;
        bcm_multicast_replication_t_init(&replications);
        replications.encap1 = encap_id;
        replications.port = access_port;
        rv = bcm_multicast_add(unit, mc_group_id, flags, 1, &replications);
        if (rv != BCM_E_NONE)
        {
          printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, access_port, encap_id);
          return rv;
        }
    }

  return rv;
}


int vxlan_l2_addr_add(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vid,
    int gport)
{
    int rv;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port = gport;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }
    return rv;
}

int
vxlan_fwd_l2(
    int unit)
{
    int rv;

    /*
     * add bridge entry to provider network
     */
    rv = vxlan_l2_addr_add (unit, cint_vxlan_basic_info.access_eth_fwd_mac, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), l2_addr_add\n", rv);
        return rv;
    }
    /*
     * add bridge entry to access network
     */
    rv = vxlan_l2_addr_add (unit, cint_vxlan_basic_info.provider_eth_fwd_mac, cint_vxlan_basic_info.vpn_id, ip_tunnel_basic_info.access_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}


/**
 * Set vxlan learn mode as: symmetric outlif + FEC
 * Update vxlan learn mode by updating tunnel term/init
 */
int update_vxlan_learn_mode_sym(int global_lif_id)
{
    int rv = 0;

    /* update tunnel term and tunnel enc so lif are symmetric: global tunnel inlif = global tunnel outlif:
       update ipv4 tunnel term id and ipv4 tunnel encap id */

    /* tunnel encap - set with id */
    ip_tunnel_basic_info.tunnel_flags[0] = BCM_TUNNEL_WITH_ID;
    /* tunnel encap - set id */
    BCM_GPORT_TUNNEL_ID_SET(ip_tunnel_basic_info.encap_tunnel_id[0], global_lif_id);

    /* tunnel term - set with id */
    ip_tunnel_basic_info.term_with_id_flag[0]= 1;
    /* tunnel term - set id */
    BCM_GPORT_TUNNEL_ID_SET(ip_tunnel_basic_info.term_tunnel_id[0], global_lif_id);
    return rv;
}

int update_vxlan_learn_mode_sym_lif_and_arp_fec(int global_lif_id) {
    int rv = 0;
    cint_vxlan_basic_info.learn_mode = LEARN_SYM_OUTLIF_FEC;

    rv = update_vxlan_learn_mode_sym(global_lif_id);

    return rv;
}

int update_vxlan_learn_mode_sym_lif_and_port(int global_lif_id) {
    int rv = 0;
    cint_vxlan_basic_info.learn_mode = LEARN_SYM_OUTLIF_PORT;

    rv = update_vxlan_learn_mode_sym(global_lif_id);

    return rv;
}

/*
 * Function that creates the fec for core side(from access to core into overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int vxlan_encap_fec_learn(
    int unit,
    int tunnel_idx)
{
    int rv = 0;
    if (is_device_or_above(unit, JERICHO2))
    {
        /*
        * JR2 IOP: use following to update FEC-ID to 18bit.
        * Only 15bit FEC-ID can be used for learning for itself and its partners.
        */
        int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
        if (!is_jericho2_mode) {
            rv = fec_id_alignment_field_wa_add(unit, ip_tunnel_basic_info.access_port, BCM_L3_ITF_VAL_GET(ip_tunnel_basic_info.encap_fec_id[tunnel_idx]));
            if (rv != BCM_E_NONE)
            {
                printf("Error, fec_id_alignment_field_wa_entry_add\n");
                return rv;
            }
        }
    }
	return rv;
}

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * provider_port - port where DC Fabric router is connected to.
 */
int vxlan_example(
    int unit,
    int access_port,
    int provider_port,
    int vdc_enable)
{

    int rv;
    int tunnel_idx = 0;
    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;

    int device_is_jericho2;
    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2\n", proc_name);
        return rv;
    }

    vxlan_basic_init(unit, access_port, provider_port, vdc_enable);

    /*
     * Build L2 VPN
     */
    rv = sand_utils_vxlan_open_vpn(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc, cint_vxlan_basic_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in sand_utils_vxlan_open_vpn, vpn=%d, vdc=%d \n", rv, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc);
        return rv;
    }

    /* Arad+ use the bounce back filter to drop the packet from vxlan to vxlan instead of split horizon */
    if (!is_device_or_above(unit,JERICHO)) {
        rv = vxlan_bounce_back_filter_init(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), vxlan_bounce_back_filter_init \n", rv);
            return rv;
        }
    }

    /*
     * Set the split horizon
     */
    rv = vxlan_split_horizon_set(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc, cint_vxlan_basic_info.vxlan_network_group_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_split_horizon_set\n", rv);
        return rv;
    }

    rv = ip_tunnel_fec_arp_map(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), ip_tunnel_fec_arp_map \n", rv);
        return rv;
    }

    /* Open route interfaces */
    rv = ip_tunnel_open_route_interfaces(unit, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), ip_tunnel_open_route_interfaces \n", rv);
        return rv;
    }


    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = ip_tunnel_term_create(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in function ip_tunnel_term_create \n", rv);
        return rv;
    }

    /*
     *Configure ARP entries
     */
    rv = ip_tunnel_create_arp(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ip_tunnel_create_arp\n", rv);
        return rv;
    }
    /*
     * Create the tunnel initiator
     */
    rv = ip_tunnel_encap_create(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ip_tunnel_encap_create \n", rv);
        return rv;
    }

    /*
     * Configure tunnel fec entry
     */
    if (cint_vxlan_basic_info.learn_mode == LEARN_FEC_ONLY) {
        rv = ip_tunnel_create_fec(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in ip_tunnel_create_fec\n", rv);
            return rv;
        }
    }

    /*
     * configure ARP fec entry for learning purpose
    */
    if (cint_vxlan_basic_info.learn_mode == LEARN_SYM_OUTLIF_FEC) {
        rv = arp_create_fec(unit, tunnel_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in arp_create_fec\n", rv);
            return rv;
        }
    }

    /*
     * configure fec for learning purpose
     */
    rv = vxlan_encap_fec_learn(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_encap_fec_learn\n", rv);
        return rv;
    }

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_vxlan_port_add(unit, ip_tunnel_basic_info.provider_port, tunnel_idx, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_vxlan_port_add \n", rv);
        return rv;
    }

    rv = vxlan_network_domain_set(unit, access_port, provider_port, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_network_domain_set \n", rv);
        return rv;
    }

    rv = vxlan_vlan_port_add(unit, access_port, cint_vxlan_basic_info.access_vid, &cint_vxlan_basic_info.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_vlan_port_add \n", rv);
        return rv;
    }

    rv = vswitch_add_port(unit, cint_vxlan_basic_info.vpn_id, access_port, cint_vxlan_basic_info.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vswitch_add_port\n", rv);
        return rv;
    }

    /* */
    /* add provider port to provider vlan. */
    int provider_vlan = 20;
    rv = bcm_vlan_gport_add(unit, provider_vlan, provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in %s(): bcm_vlan_gport_add \n");
        return rv;
    }


    rv = vxlan_fwd_l2(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_fwd_l2 \n", rv);
        return rv;
    }

    return rv;
}

