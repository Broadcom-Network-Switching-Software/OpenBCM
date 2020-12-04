/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_src_mac_encap.c
 * Purpose: Example L2 source mac encapsulation.
 *
 * Fuctions:
 * Main function:
 *      l2_src_encap_example_dvapi()
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint ../../src/examples/sand/cint_sand_src_mac_encap.c
 * cint
 * l2_src_encap_example_dvapi(units_ids,1,1811939528,1811939529);
 * exit;
 *
 * /* IPv4oETH1 */
 * tx 1 psrc=200 data=0x000c000200000000070001008100000f080045000035000000008000b1c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x00000000cd1d00000000cd1581000065080045000035000000007f00b2c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 */

int src_encap_out_vlan_port_id;
int
ac_action_add (
    int unit,
    int out_vlan,
    int out_sysport)
{
    int rv;
    int is_adv_vt, action_id;
    bcm_vlan_action_set_t action;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t port_match_info;
    bcm_vlan_translate_action_class_t action_class;

    is_adv_vt = is_advanced_vlan_translation_mode(unit);
    if (is_adv_vt)
    {
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = out_sysport;
        vlan_port.vsi = out_vlan;
        vlan_port.flags = BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        if (is_device_or_above(unit, JERICHO2)) {
            /* On JR2, BCM_VLAN_PORT_VLAN_TRANSLATION can't be used for both ingress and egress in the same call
             * On JR1, Egress only VLAN Port has to be with match criteria NONE
             */
            vlan_port.flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
        }
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
        }

        src_encap_out_vlan_port_id = vlan_port.vlan_port_id;
        rv = vlan__port_translation__set(unit, (out_vlan+1), 0, vlan_port.vlan_port_id, 2, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set\n");
        }

        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
        }

        bcm_vlan_action_set_t_init(&action);
        action.outer_tpid = 0x8100;
        action.dt_outer = bcmVlanActionAdd;
        action.new_outer_vlan = out_vlan+1;
        action.dt_outer_pkt_prio = bcmVlanActionAdd;
        rv = bcm_vlan_translate_action_id_set(unit,BCM_VLAN_ACTION_SET_EGRESS, action_id, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
        }

        action_class.vlan_edit_class_id = 2;
        action_class.tag_format_class_id = 0;
        action_class.vlan_translation_action_id = action_id;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv= bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
        }
    }
    else
    {
        bcm_vlan_port_t_init(&vlan_port);

        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.match_vlan = out_vlan;
        vlan_port.port = out_sysport;
        vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : (out_vlan + 1);

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
        }

        port_match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
        port_match_info.match = BCM_PORT_MATCH_PORT;
        port_match_info.port = out_sysport;
        port_match_info.match_vlan = BCM_VLAN_ALL;

        rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &port_match_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_match_add\n");
        }
    }

    return rv;
}

int
l2_src_encap_example_dvapi (
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport)
{
    int rv;
    int i, unit, host;
    int ing_intf_in, ing_intf_out;
    int fec[2] = {0x0,0x0};
    int flags = 0, flags1 = 0;
    int in_vlan = 15, out_vlan = 100, open_vlan = 1;
    int vrf = 0;
    int encap_id[2]={0x0,0x0};
    int route, mask;
    bcm_mac_t mac_address  = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  /* my-MAC */
    bcm_mac_t next_hop_mac  = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac1 */
    bcm_mac_t custom_mac = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x15}; /* Custom mac */
    sand_utils_l3_eth_rif_s eth_rif;
    l3_egress_s l3_egress;
    sand_utils_l3_route_ipv4_s route_ipv4;

    flags = 0;
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        }

        rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan);
            return rv;
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif, in_vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, mac_address, vrf);
        eth_rif.urpf_mode = L3_uc_rpf_mode;
        eth_rif.mtu_valid  = 1;
        eth_rif.mtu  = default_mtu;
        eth_rif.urpf_mode = L3_uc_rpf_mode;
        if (!is_device_or_above(unit, JERICHO2)) {
            eth_rif.mtu_forwarding = default_mtu_forwarding;
        }
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            return rv;
        }
        ing_intf_in = eth_rif.l3_rif;
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    flags = 0;
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    for (i = 0; i < nof_units; i++){
        unit = units_ids[i];

        rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
        }
        rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
            return rv;
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif, out_vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, mac_address, vrf);
        eth_rif.urpf_mode = L3_uc_rpf_mode;
        eth_rif.mtu_valid  = 1;
        eth_rif.mtu  = default_mtu;
        eth_rif.urpf_mode = L3_uc_rpf_mode;
        if (!is_device_or_above(unit, JERICHO2)) {
            eth_rif.mtu_forwarding = default_mtu_forwarding;
        }
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            return rv;
        }
        ing_intf_out = eth_rif.l3_rif;
    }

    l3_egress.allocation_flags = 0;
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];

        if (is_device_or_above(unit, JERICHO2))
        {
            /*
             * On JR2,
             * 1) Src mac is configurad on arp, bcmTunnelTypeL2SrcEncap is not supported.
             * 2) cascaded_fec is no needed
             */
            sand_utils_l3_arp_s l3_arp;
            sand_utils_l3_fec_s l3_fec;

            sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[1], 0, BCM_L3_FLAGS2_SRC_MAC, next_hop_mac, out_vlan);
            l3_arp.src_mac_addr = custom_mac;
            rv = sand_utils_l3_arp_create(unit, &l3_arp);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create AC egress object ARP only\n");
                return rv;
            }
            encap_id[1] = l3_arp.l3eg_arp_id;;

            /* Create FEC and set its properties */
            sand_utils_l3_fec_s_common_init(unit, 0/*allocation_flags*/, 0, l3_fec, fec[1], 0/*l3_flags*/, 0/*l3_flags2*/, out_sysport, ing_intf_out, encap_id[1]);
            rv = sand_utils_l3_fec_create(unit, &l3_fec);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create egress object FEC only\n");
                return rv;
            }
            fec[1] = l3_fec.l3eg_fec_id;
            fec_id_used = fec[1];
        }
        else
        {
            int fwd_group_cascaded_fec;
            bcm_tunnel_initiator_t tunnel;
            bcm_failover_t failover_id_fec = 0;

            bcm_l3_intf_t l3_intf_tunnel;
            bcm_l3_intf_t_init(&l3_intf_tunnel);
            bcm_tunnel_initiator_t_init(&tunnel);
            tunnel.type = bcmTunnelTypeL2SrcEncap;
            tunnel.smac = custom_mac;
            rv = bcm_tunnel_initiator_create(unit, &l3_intf_tunnel, &tunnel);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_tunnel_initiator_create\n");
            }

            l3_egress.l3_flags = BCM_L3_CASCADED;
            l3_egress.allocation_flags = BCM_L3_CASCADED;
            l3_egress.out_gport = out_sysport;
            l3_egress.vlan = out_vlan;
            l3_egress.next_hop_mac_addr = next_hop_mac;
            l3_egress.out_tunnel_or_rif = l3_intf_tunnel.l3a_intf_id;
            l3_egress.arp_encap_id = encap_id[1];

            l3_egress.fec_id = fec[1];
            rv = sand_utils_l3_egress_create(unit, &l3_egress);
            if (rv != BCM_E_NONE) {
                printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
            }

            encap_id[1] = l3_egress.arp_encap_id;
            BCM_GPORT_FORWARD_PORT_SET(fwd_group_cascaded_fec, l3_egress.fec_id);

            l3_egress.out_tunnel_or_rif = ing_intf_out;
            l3_egress.out_gport = fwd_group_cascaded_fec;
            l3_egress.vlan = out_vlan;
            l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
            l3_egress.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
            l3_egress.fec_id = fec[1];
            if (!is_device_or_above(unit,JERICHO_B0)) {
                rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_failover_create\n");
                    return rv;
                }
            }

            l3_egress.failover_id = failover_id_fec;
            l3_egress.failover_if_id = 0;

            if (!is_device_or_above(unit,JERICHO_B0)) {
                l3_egress.arp_encap_id = 0;
                rv = sand_utils_l3_egress_create(unit,&l3_egress);
                if (rv != BCM_E_NONE) {
                    printf("Error, l3__egress__create\n");
                    return rv;
                }
                l3_egress.failover_if_id = l3_egress.fec_id;
            }

            l3_egress.arp_encap_id = encap_id[1];
            rv = sand_utils_l3_egress_create(unit, &l3_egress);
            if (rv != BCM_E_NONE) {
                printf("Error, l3__egress__create\n");
                return rv;
            }

            fec[1] = l3_egress.fec_id;
            fec_id_used = fec[1];
            if(verbose >= 1) {
                printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
                printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
            }
        }

        rv = ac_action_add(unit, out_vlan, out_sysport);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_action_add\n");
            return rv;
        }
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    if(soc_property_get(unit , "enhanced_fib_scale_prefix_length",0)){
        mask  = 0xffff0000;
    }
    else{
        mask  = 0xfffffff0;
    }
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        printf("fec used is  = 0x%x (%d)\n", fec[1], fec[1]);

        sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4, route, mask, vrf, 0, 0, fec[1]);
        rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4);
        if (rv != BCM_E_NONE) {
            printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
            return rv;
        }
    }

    return rv;
}


