/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Test Scenario: flooding, UC packets
 *
 *  
 * Network diagram
 * 
 * We configure Router A
 *                                          _________
 *                                         | ROUTER B|------host1             
 *                                host2    |_________|                        
 *                                    |       |access_vlan_1, access_port_1                
 *                ____________________|_______|_______________________________
 *               |                   |         |                             |
 *               |                   | ROUTER A| (intra DC IP)    DC FABRIC  |
 *               | native_out_vlan   |_________|                             |
 *               | provider_vlan,      /                                     |
 *               | provider_vlan_2    /                                      |
 *               | vni1       _______/___                                    |
 *               |           |          |                                    |
 *               |           | ROUTER C |                                    |
 *               |           |__________|                                    |
 *               |            /                                              |
 *               |         /                                                 |
 *               |_____ /____________________________________________________|
 *                ____/___                                          
 *               |  TOR1 |                                   
 *               |_______|                                   
 *               |       |                                   
 *               |       |                                   
 *               |_______|                                   
 *               | VM:A0 |                                   
 *               |_______|      
 *
 * soc properties (for Jericho devices only. Jericho2 doesn't need any soc property):
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
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ecmp_basic.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_ip_tunnel.c
 * cint ../../src/examples/sand/cint_sand_vxlan.c
 * cint ../../src/examples/sand/cint_sand_vxlan_roo.c
 * cint
 * vxlan_roo_example(0,0,200,202,1);
 * exit;
 *
 * Traffic: 
 * vxlan - packet from user to DC fabric using L3 host 
 *  Purpose: - check VXLAN tunnel encapsulation
 * Packet flow:
 * - get VSI from port default VSI
 * - packet is routed: forwarded according to DA and VRF
 *   - vxlan tunnel is built using IP tunnel eedb entry.     
 *   - vxlan header.vni is resovled from VSI -> vni
 *   - ethernet header is built using LL eedb entry (IP tunnel eedb entry point to LL eedb entry): 
 *   - DA from LL eedb entry, SA from LL eedb entry.VSI.myMac
 *   - VLAN = entry.VSI
 * 
 * tx 1 psrc=200 data=0x000c0002000000000000058181000005080045000035000000008000b7c0010203047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be: 
 * 0x00000000cd1d000c000200008100800a08004528006b000000003c111d1aa00f1011a10f1011d9d012b500570000080000000013880001000000cd1d000c000200228100000f080045000035000000007f00b8c0010203047fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * vxlan - packet from DC fabric to user using L3 host
 * Purpose: - check VXLAN tunnel termination
 * Packet flow:
 * - tunnel is terminated
 * - native my mac is terminated
 * - packet is routed: forwarded according to DA and VRF
 * 
 * tx 1 psrc=202 data=0x000c000200000000000005818100000a08004500006b000000008011d941a00f1011a10f1011c35112b5005700000800000000138800000c0002002220000000cd1d8100000f08004500003500000000800035b30102030401000011000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be: 
 * 0x000c00020001000c0002000081000005080045000035000000007f0036b30102030401000011000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * 
 *
  * Traffic: 
 * vxlan - packet from user to DC fabric using L3 routing 
 *  Purpose: - check VXLAN tunnel encapsulation
 * Packet flow:
 * - get VSI from port default VSI
 * - packet is routed: forwarded according to DA and VRF
 *   - vxlan tunnel is built using IP tunnel eedb entry.     
 *   - vxlan header.vni is resovled from VSI -> vni
 *   - ethernet header is built using LL eedb entry (IP tunnel eedb entry point to LL eedb entry): 
 *   - DA from LL eedb entry, SA from LL eedb entry.VSI.myMac
 *   - VLAN = entry.VSI
 * 
 * tx 1 psrc=200 data=0x000c0002000000000000058181000005080045000035000000008000b7c3010203047fffff00000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be: 
 *  0x00000000cd1d000c000200008100800a08004528006b000000003c111d1aa00f1011a10f1011e4b012b500570000080000000013880021000000cd1d000c000200228100000f080045000035000000007f00b8c3010203047fffff00000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * 
 * 
 * vxlan - packet from DC fabric to user using L3 routing
 * Purpose: - check VXLAN tunnel termination
 * Packet flow:
 * - tunnel is terminated
 * - native my mac is terminated
 * - packet is routed: forwarded according to DA and VRF
 * 
 * tx 1 psrc=202 data=0x000c000200000000000005818100000a08004500006b000000008011d941a00f1011a10f1011c35112b5005700000800000000138800000c0002002220000000cd1d8100000f08004500003500000000800035ba010203040100000a000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be: 
 * 0x000c00020001000c0002000081000005080045000035000000007f0036ba010203040100000a000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

static int l3_in_map_id;
static int l3_eg_map_id;

static int l3_in_opcode_id;
static int l3_eg_opcode_id;
int
vxlan_roo_init(
    int unit,
    int is_ipv6,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;

    ip_tunnel_basic_info.roo = TRUE; 
    ip_tunnel_basic_info.tunnel_type[0] = is_ipv6 ? bcmTunnelTypeVxlan6 : bcmTunnelTypeVxlan;
    ip_tunnel_basic_info.access_port = access_port;
    ip_tunnel_basic_info.provider_port = provider_port;   
    if (is_device_or_above(unit,JERICHO)) {
        ip_tunnel_basic_info.encap_is_hierarchy[0] = TRUE; 
        ip_tunnel_basic_info.encap_is_hierarchy[1] = TRUE; 
    }

    /* init ip tunnel termination global 
       skip ethernet flag is available until arad+. 
       For jericho, support 2nd my mac termination for native ethernet termination. */
    ip_tunnel_basic_info.skip_ethernet_flag = (!is_device_or_above(unit,JERICHO));
    printf("skip ethernet flag: %d \n", ip_tunnel_basic_info.skip_ethernet_flag);
    
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
vxlan_roo_qos_init(
    int unit,
    int ing_qos_id,
    int egr_qos_id)
{
    int rv = BCM_E_NONE;
    
    ip_tunnel_basic_info.ing_qos_id = ing_qos_id;
    ip_tunnel_basic_info.egr_qos_id = egr_qos_id;
    ip_tunnel_basic_info.dscp_sel[0] = bcmTunnelDscpAssign;
    ip_tunnel_basic_info.encap_flag = 1; /* use the arp+ac for outer arp */

    return rv;
}

/* For Jer+ and above devices.
   Create a native AC-LIF.
   Configure it to add 1 native vlans
   Configure it as default AC. */
int vxlan_roo_native_default_out_ac(int unit) {
    int rv = BCM_E_NONE;
    int inner_vlan;
    vlan_action_utils_s action;

    action.o_tpid = 0x8100;
    action.i_tpid = 0x9100;
    action.o_action = bcmVlanActionAdd; /* outer action */
    action.i_action = bcmVlanActionNone; /* outer action */

    /* VIRTUAL EGERSS DEFUALT thus new inner vlan can not be set */
    inner_vlan = is_device_or_above(unit, JERICHO2) ? 0 : 21;
    rv = vlan__native_default_out_ac_allocate_and_set(unit, 20, inner_vlan, &action, ip_tunnel_basic_info.encap_tunnel_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__native_default_out_ac_allocate_and_set\n");
        return rv;
    }

    cint_vxlan_basic_info.native_vlan_action_id = action.action_id;

    return rv;
}

/* For Jericho/Qmx devices only.
   Create an out ac. Used to perform native VE. */
 int vxlan_roo_out_ac_perform_native_ve(int unit, int access_port, int provider_port) {

    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = access_port;
    vlan_port.match_vlan = 20;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : 20;
    vlan_port.vsi = cint_vxlan_basic_info.vpn_id;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: 0x%x\n", rv);
        return rv;
    }

    rv = vswitch_add_port(unit, cint_vxlan_basic_info.vpn_id, provider_port, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
       are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        /* set egress vlan translation: double tagged */
        rv = vlan__eve_default__set(unit, vlan_port.vlan_port_id, 20, 21, bcmVlanActionAdd, bcmVlanActionNone);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan__eve_default__set\n");
        }
        printf("For advance vlan editing: default out AC : %d. \n", vlan_port.vlan_port_id);
    }

    return rv;
}

/* modify current configuration to build the native ethernet header with specific number of vlans:
 * - update EVE. Add "nbr_native_vlans" vlans. (up to 2 vlans)
 * - update native vsi compensation: number of expected native vlan for the vsi.
 *    Number of native vlan is specified in EVE. EVE is applied after UDP is built,
 *   so UDP header size field is calculated according to native vsi compensation.
 */
int vxlan_roo_modify_native_vlan_number(int unit, int nbr_native_vlans) {
   int rv;
   uint32 flags = BCM_VLAN_ACTION_SET_EGRESS;
   bcm_vlan_action_set_t action;
   int action_id;

   bcm_vlan_action_t outer_action, inner_action;
   if (nbr_native_vlans == 2) {
       outer_action = bcmVlanActionAdd;
       inner_action = bcmVlanActionAdd;
   } else if (nbr_native_vlans == 1) {
       outer_action = bcmVlanActionAdd;
       inner_action = bcmVlanActionNone;
   } else if (nbr_native_vlans == 0) {
       outer_action = bcmVlanActionNone;
       inner_action = bcmVlanActionNone;
   } else {
       printf("Error, invalid number of native vlans \n");
       return rv;
   }

   bcm_vlan_action_set_t_init(&action);
   action.dt_outer = outer_action;
   action.dt_inner = inner_action;
   action.outer_tpid = 0x8100;
   action.inner_tpid = 0x9100;

   action_id = is_device_or_above(unit,JERICHO_PLUS) ? cint_vxlan_basic_info.native_vlan_action_id : g_eve_edit_utils.action_id;

   /* For JR+,  action_id for action from native VE */
   rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
   if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_translate_action_id_set \n");
      return rv;
   }

   /* update native vsi compensation */
   /* use replace functionality to update the native vsi compensation */
   if (is_device_or_above(unit, ARAD_PLUS) && !is_device_or_above(unit,JERICHO_PLUS)) {

       bcm_l3_intf_t l3if;
       bcm_l3_intf_t_init(&l3if);
       l3if.l3a_intf_id = cint_vxlan_basic_info.vpn_id; /* vpn is used as native outRif*/
       rv = bcm_l3_intf_get(unit, &l3if);
       if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_intf_get \n");
            return rv;
       }
       l3if.l3a_flags = l3if.l3a_flags | BCM_L3_REPLACE | BCM_L3_WITH_ID;
       l3if.native_routing_vlan_tags = nbr_native_vlans;
       rv = bcm_l3_intf_create(unit, &l3if);
       if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_intf_create \n");
            return rv;
       }
   }
   return rv;
}

int vxlan_my_mac_vrrp(
    int unit)
{
    int rv = BCM_E_NONE;
    int is_arad= 0;
    int vrrp_ip_flags[3] = {BCM_L3_VRRP_IPV4,BCM_L3_VRRP_IPV6, BCM_L3_VRRP_IPV4|BCM_L3_VRRP_IPV6};
    /* 
     *  Configure 00:00:5e:00:01:77 for IPv4 and 00:00:5e:00:02:77 for IPv6 as a mymac address for all VSIs.
     *  The regular Router A mac is configured as mymac when calling bcm_l3_intf_create.
     *  Devices below JR2 also have the option of supporting 00:00:5e:00:01:77 for IPv6.
 */
    int vrid = 0x77;
    int ip_flag_iter;
    int nof_iters = is_device_or_above(unit, JERICHO2) ? 2 : 3;
    rv = is_arad_plus_only(unit, &is_arad) ? 2 : 0;
    if (rv != BCM_E_NONE) {
        printf("Error, in is_arad_plus_only. \n");
        return rv;
    }
    int start_iter = is_arad ? 2 : 0;

    for(ip_flag_iter = start_iter; ip_flag_iter < nof_iters; ip_flag_iter++)
    {
        rv = bcm_l3_vrrp_config_add(unit, vrrp_ip_flags[ip_flag_iter] , ip_tunnel_basic_info.access_eth_rif, vrid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_vrrp_config_add. \n");
            return rv;
        }

        rv = bcm_l3_vrrp_config_add(unit, vrrp_ip_flags[ip_flag_iter] , ip_tunnel_basic_info.provider_eth_rif, vrid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_vrrp_config_add. \n");
            return rv;
        }

        rv = bcm_l3_vrrp_config_add(unit, vrrp_ip_flags[ip_flag_iter] , ip_tunnel_basic_info.provider_native_eth_rif, vrid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_vrrp_config_add. \n");
            return rv;
        }
    }

    return rv;
}

/* create ecmp: ECMP points to ip tunnel FEC 
  * Jericho note: 
  * host entry can hold up to 12b of FEC id (4k ids)                                                    
  * In Jericho, ECMP is also 4k.                                                                        
  * Consequently, to add a host entry in jericho, we'll have to add a ECMP entry instead of the FEC. 
  */ 
int vxlan_second_level_ecmp_add(
    int unit,
    int tunnel_idx)
{
    int rv;
    /* create ecmp: ECMP points to ip tunnel FEC 
     * Jericho note: 
     * host entry can hold up to 12b of FEC id (4k ids)                                                    
     * In Jericho, ECMP is also 4k.                                                                        
     * Consequently, to add a host entry in jericho, we'll have to add a ECMP entry instead of the FEC. */  
    bcm_l3_egress_ecmp_t ecmp; 
    int ecmp_nof_paths; 
    bcm_if_t intfs[2];
    int flags = 0;
    bcm_l3_egress_ecmp_t_init(&ecmp);

    if (cint_vxlan_basic_info.second_level_ecmp_enable) {
        ecmp_nof_paths = 2; 
        intfs[0] = ip_tunnel_basic_info.encap_fec_id[tunnel_idx];
        intfs[1] = ip_tunnel_basic_info.encap_fec_id[tunnel_idx + 1];
    }
    else {
        ecmp_nof_paths = 1;
        intfs[0] = ip_tunnel_basic_info.encap_fec_id[tunnel_idx];
    }

    if (is_device_or_above(unit,JERICHO2)) {
        flags = BCM_L3_2ND_HIERARCHY;
    } 
    rv = l3__ecmp_create(unit, 0, ecmp_nof_paths, &intfs[0], flags,
                         BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__ecmp_create\n");
        return rv;
    }

    ip_tunnel_basic_info.encap_ecmp[tunnel_idx] = ecmp.ecmp_intf;
    
    return rv;
}

/*
 * Add L3 forward host entry:
 *     for encapsulation: 
 *         add host entry: add entry through vxlan overlay in host table using the payload: FEC + ARP + OutRIF. 
 *
 *     for termination: 
 *         add host entry: add entry out vxlan overlay in host table using the payload: FEC.
 * Unit - relevant unit
 */
int
vxlan_fwd_l3_host(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    int device_is_jericho2;
    int opportunistic_learning_always_transplant = 0;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }

    /*
     * Add Host entry for access to core traffic
     * Jericho host entry can hold up to 12b of FEC id (4k ids), we'll have to add a ECMP entry instead of the FEC. 
     * Jericho2 don't have this issue, add the FEC entry directly.
     */
    if (device_is_jericho2)
    {
        opportunistic_learning_always_transplant = *(dnxc_data_get(unit, "l2", "feature", "opportunistic_learning_always_transplant", NULL));
        if (opportunistic_learning_always_transplant)
        {
            rv = add_route_ipv4(unit, ip_tunnel_basic_info.encap_host_dip,
                                0xffffffff,
                                ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_native_fec_id[0]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in add_route_ipv4, \n");
                return rv;
            }
        }
        else 
        {
            rv = add_host_ipv4(unit, ip_tunnel_basic_info.encap_host_dip, ip_tunnel_basic_info.access_eth_vrf, 
                ip_tunnel_basic_info.encap_native_fec_id[0], 0, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in add_host_ipv4, \n");
                return rv;
            }
        }
    } 
    else
    {
        BCM_GPORT_FORWARD_PORT_SET(gport, ip_tunnel_basic_info.encap_ecmp[0]);
        rv = add_host_ipv4(unit, ip_tunnel_basic_info.encap_host_dip, ip_tunnel_basic_info.access_eth_vrf, 
            ip_tunnel_basic_info.provider_native_eth_rif, ip_tunnel_basic_info.encap_native_arp_id[0], gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in add_host_ipv4, \n");
            return rv;
        }
    }    

    /*
     * Add Host entry for core to access traffic
     */
    if (opportunistic_learning_always_transplant)
    {
        rv = add_route_ipv4(unit, ip_tunnel_basic_info.term_host_dip[0],
                            0xffffffff,
                            ip_tunnel_basic_info.provider_native_eth_vrf, ip_tunnel_basic_info.term_fec_id[0]);

        if (rv != BCM_E_NONE)
        {
            printf("Error, in add_route_ipv4, \n");
            return rv;
        }

    }
    else 
    {
        rv = add_host_ipv4(unit, ip_tunnel_basic_info.term_host_dip[0], ip_tunnel_basic_info.provider_native_eth_vrf, ip_tunnel_basic_info.term_fec_id[0], 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in add_host_ipv4, \n");
            return rv;
        }
    }

    return rv;
}

/*
 * Add L3 forward route entry:
 *     for encapsulation: 
 *         add LPM entry: add entry through vxlan overlay in route table using cascaded fec. 
 *         For cascaded fec, only need to provide HI-FEC: 
 *         hi fec contains native out rif, native arp pointer and point to overlay fec
 *         JR2: 
 *              route -> native_fec
 *              native_fec -> overlay_fec
 *         JER and Arad+: 
 *              route -> native_fec
 *              native_fec -> encap_ecmp
 *              encap_ecmp -> overlay_fec
 * Unit - relevant unit
 */
int
vxlan_fwd_l3_route(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;

    if (is_device_or_above(unit,JERICHO)) {
        /*
         * Add Route entry for access to core traffic
         */
        rv = add_route_ipv4(unit, ip_tunnel_basic_info.encap_host_dip,
                           ip_tunnel_basic_info.encap_route_dip_mask,
                             ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_native_fec_id[1]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4 \n");
            return rv;
        }
        
        rv = add_route_ipv6(unit, ip_tunnel_basic_info.encap_host_dip6,
                           ip_tunnel_basic_info.encap_route_dip6_mask,
                             ip_tunnel_basic_info.access_eth_vrf, ip_tunnel_basic_info.encap_native_fec_id[1]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv6 \n");
            return rv;
        }

        /*
         * Add Route entry for core to access traffic
         */
        rv = add_route_ipv4(unit, ip_tunnel_basic_info.term_host_dip[0],
                            ip_tunnel_basic_info.term_route_dip_mask,
                            ip_tunnel_basic_info.provider_native_eth_vrf, ip_tunnel_basic_info.term_fec_id[0]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4 \n");
            return rv;
        }

        rv = add_route_ipv6(unit, ip_tunnel_basic_info.term_host_dip6[0],
                           ip_tunnel_basic_info.term_route_dip6_mask,
                             ip_tunnel_basic_info.provider_native_eth_vrf, ip_tunnel_basic_info.term_fec_id[0]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv6 \n");
            return rv;
        }
    }

    return rv;
}

int
vxlan_fwd_l3(
    int unit)
{
    int rv = BCM_E_NONE;

    rv = vxlan_fwd_l3_host(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vxlan_fwd_l3_host\n");
        return rv;
    }

    if (cint_vxlan_basic_info.add_routing_table_entry) {
        rv = vxlan_fwd_l3_route(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, vxlan_fwd_l3_route\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Main function for qos ingress PHB and remark setting:
 *
 * Map ingress as follows
 * Ingress DSCP     INTERNAL TC         INTERNAL DP     INGRESS NWK-QOS
 *                  = DSCP % 8          = DSCP % 2      = DSCP - 1
 *     0            0                   GREEN           255
 *     1            1                   YELLOW          0
 *     2            2                   GREEN           1
 *     3            3                   YELLOW          2
 *     *            *                   *               *
 *     60           4                   GREEN           59
 *     61           5                   YELLOW          60
 *     62           6                   GREEN           61
 *     63           7                   YELLOW          62
 * 
 * For JR1:
 *     1. Create QOS ingress profile
 *     2. Set the qos mapping: DSCP -> TC, DP, in-DSCP-EXP.
 * For JR2:
 *     1. Create QOS ingress profile for PHB and remark.
 *     2. Create QOS ingress opcode for PHB
 *     3. Set the qos mapping: ingress profile, IPv4 -> ingress opcode (PHB_PROFILE_TO_INTERNAL_MAP)
 *     4. Set the qos mapping: DSCP -> TC, DP
 *     5. Create QOS ingress opcode for remark
 *     6. Set the qos mapping: ingress profile, IPV4 -> ingress opcode(REMARK_PROFILE_TO_INTER_MAP)
 *     7. Set the qos mapping: DSCP -> NWK_QOS
 */
int
qos_map_ingress_phb_remark_set(int unit)
{
    bcm_qos_map_t l3_in_map;
    int dscp;
    int rv = BCM_E_NONE;
    uint32 flags;    

    /** 1. Create qos-profile */
    flags = BCM_QOS_MAP_INGRESS;
    if (is_device_or_above(unit,JERICHO2)) {
        flags |= BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for ingress cos-profile\n", rv);
        return rv;
    }
    printf("bcm_qos_map_create l3_in_map_id: %x\n", l3_in_map_id);


    if (is_device_or_above(unit,JERICHO2)) {
        /** Create opcode for PHB */
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE, &l3_in_opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_create() for ingress opcode\n", rv);
            return rv;
        }
        printf("bcm_qos_map_create ingress PHB opcode: %x\n", l3_in_opcode_id);

        /** Add the maps for PHB */
        bcm_qos_map_t_init(&l3_in_map);
        l3_in_map.opcode = l3_in_opcode_id;
        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE, &l3_in_map, l3_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode for PHB\n", rv);
            return rv;
        }

        /** Add maps from {packet.qos & opcode} to {tc,dp} for JR2 only*/
        for (dscp = 0; dscp < 256; dscp++) {
            bcm_qos_map_t_init(&l3_in_map);
			l3_in_map.dscp = dscp; /* packet DSCP (TOS) */
            l3_in_map.int_pri = dscp % 8; /* TC */
            l3_in_map.color = dscp % 2; /* DP */
            l3_in_map.remark_int_pri = (dscp - 1) & 0xff; /* in-DSCP-exp */

            if (is_device_or_above(unit,JERICHO2)) {
                rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB, &l3_in_map, l3_in_opcode_id);
            } else {
                rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB, &l3_in_map, l3_in_map_id);
            }
            if (rv != BCM_E_NONE) {
                printf("Error, set QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
                return rv;
            }
        }

        /** Create opcode for remark*/
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &l3_in_opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_create() for ingress remark opcode\n", rv);
            return rv;
        }    
        printf("bcm_qos_map_create ingress REMARK opcode: %x\n", l3_in_opcode_id);
               
        /** Add the maps for remarking */
        bcm_qos_map_t_init(&l3_in_map);
        l3_in_map.opcode = l3_in_opcode_id;    
        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &l3_in_map, l3_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode for remarking\n", rv);
            return rv;
        }
    }

    /* set QoS mapping for L3 in ingress:
       map In-DSCP-EXP = IP-Header.TOS - 1 */
    for (dscp = 0; dscp < 256; dscp++) {
        bcm_qos_map_t_init(&l3_in_map);
        l3_in_map.dscp = dscp; /* packet DSCP (TOS) */
        l3_in_map.int_pri = dscp % 8; /* TC */
        l3_in_map.color = dscp % 2; /* DP */
        l3_in_map.remark_int_pri = (dscp - 1) & 0xff; /* in-DSCP-exp */
        
        if (is_device_or_above(unit,JERICHO2)) {
            rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK, &l3_in_map, l3_in_opcode_id);
        } else {
            rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &l3_in_map, l3_in_map_id);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, set QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
            return rv;
        }
    }
    printf("set ingress QoS mapping for L3\n");

    return rv;
}

/*
 * Main function for qos egress remark setting:
 *
 * Map egress as follows
 * For JR1:
 * INTERNAL TC      INTERNAL DP     EGRESS PFI      EGRESS CFI
 *                                  = TC + 1        = DP
 *     0            GREEN           1               0
 *     1            YELLOW          2               1
 *     2            GREEN           3               0
 *     3            YELLOW          4               1
 *     4            GREEN           5               0
 *     5            YELLOW          6               1
 *     6            GREEN           7               0
 *     7            YELLOW          7(TC)           1
 *
 * INTERNAL DSCP-EXP     EGRESS PFI      EGRESS CFI
 *                       = TC + 1        = DP
 *     0                 2               1
 *     1                 2               1
 *     *                 *               *
 *     62                2               1
 *     63                2               1
 *
 *  For JR2:
 * INTERNAL NWK_QOS      INTERNAL DP     EGRESS PFI      EGRESS CFI
 *                                       = TC + 1        = DP
 *     0                 GREEN           2               1
 *     0                 YELLOW          2               1
 *     1                 GREEN           2               1
 *     1                 YELLOW          2               1
 *     *                 *               *               *
 *     62                GREEN           2               1
 *     62                YELLOW          2               1
 *     63                GREEN           2               1
 *     63                YELLOW          2               1
 * 
 * For JR1:
 *     1. Create QOS egress profile
 *     2.1 Set the qos mapping: in-DSCP-EXP->PFI, CFI
 *     2.2 Set the qos mapping: TC, DP->PFI, CFI
 * For JR2:
 *     1. Create QOS egress profile.
 *     2. Create QOS egress opcode for PHB
 *     3. Set the qos mapping: egress profile, PCP_DEI -> egress opcode (EGRESS_PROFILE_TO_INTERNAL_MAP)
 *     4. Set the qos mapping: NWK_QOS, DP -> PFI, CFI
 */
int
qos_map_egress_phb_remark_set(int unit, int dscp_map)
{
    int rv;    
    int flags = 0;
    bcm_qos_map_t l3_eg_map;
    int dscp;
    int tc,dp;    

    /** 1. Create qos-profile */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2_VLAN_PCP;
    if (is_device_or_above(unit,JERICHO2)) {
        flags |= BCM_QOS_MAP_REMARK;
    }
    rv = bcm_qos_map_create(unit, flags, &l3_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for egress cos-profile\n", rv);
    } 

    printf("bcm_qos_map_create egress cos-profile: %x\n", l3_eg_map_id);

    if (is_device_or_above(unit,JERICHO2)) {
        /** 2. Create opcode */
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &l3_eg_opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error  (%d) in bcm_qos_map_create() for egress opcode\n", rv);
            return rv;
        }

        printf("bcm_qos_map_create egress opcode: %x\n", l3_eg_opcode_id);

        bcm_qos_map_t_init(&l3_eg_map);
        l3_eg_map.opcode = l3_eg_opcode_id;

        /** Add the maps for remarking */
        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &l3_eg_map, l3_eg_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    if (dscp_map) {
        if (is_device_or_above(unit,JERICHO2)) {
            /* set QoS mapping for L2: map DSCP => PCP, DEIin */
            for (dscp = 0; dscp < 256; dscp++) {
                for (dp = 0; dp < 2; dp++) {
                    bcm_qos_map_t_init(&l3_eg_map);
                    l3_eg_map.int_pri = dscp; 
                    l3_eg_map.color = dp; 
                    l3_eg_map.pkt_pri = 2; /* PCP */
                    l3_eg_map.pkt_cfi = 1; /* DEI */

                    rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK, &l3_eg_map, l3_eg_opcode_id);
                    if (rv != BCM_E_NONE) {
                        printf("Error, set L3 PCP QoS mapping, bcm_qos_map_add, dscp=%d, dp=%d\n", dscp, dp);
                        return rv;
                    }
                }
            }
        } else {
            /* set QoS mapping for L2: map DSCP => PCP, DEIin */
            for (dscp = 0; dscp < 256; dscp++) {
                bcm_qos_map_t_init(&l3_eg_map);
                l3_eg_map.dscp = dscp; /* packet DSCP (TOS) */
                l3_eg_map.pkt_pri = 2; /* PCP */
                l3_eg_map.pkt_cfi = 1; /* DEI */

                rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_L2_VLAN_PCP, &l3_eg_map, l3_eg_map_id);
                if (rv != BCM_E_NONE) {
                    printf("Error, set L3 PCP QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
                    return rv;
                }
            }
        }
    } else {
        for (tc=0; tc<8; tc++) {
            for (dp=0; dp<2; dp++) {
                flags = BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP;
                if (tc != 7) { 
                    /* Set TC -> PCP  */
                    l3_eg_map.int_pri = tc; /* TC Input */
                    l3_eg_map.color = dp; /* Color Input */
                    l3_eg_map.pkt_pri = tc + 1; /* PCP Output */
                    l3_eg_map.pkt_cfi = dp; /* CFI Output */
                } else { 
                    /* Set TC -> PCP  */
                    l3_eg_map.int_pri = tc; /* TC Input */
                    l3_eg_map.color = dp; /* Color Input */
                    l3_eg_map.pkt_pri = tc; /* PCP Output */
                    l3_eg_map.pkt_cfi = dp; /* CFI Output */
                }

                rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);    
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_qos_map_add\n");
                    return rv;
                }
            }
        }  
    }
    
    return rv;
}

/*
 * Main function for qos egress remark destory:
 * 
 * 
 * For JR1:
 *     1.1 Delete the qos mapping: in-DSCP-EXP->PFI, CFI
 *     1.2 Delete the qos mapping: TC, DP->PFI, CFI
 *     2. Delete QOS egress profile
 * For JR2:
 *     1. Delete  the qos mapping: egress profile, PCP_DEI -> egress opcode (EGRESS_PROFILE_TO_INTERNAL_MAP).
 *     2. Delete  the qos mapping: NWK_QOS, DP -> PFI, CFI
 *     3. Delete QOS egress opcode for PHB
 *     4. Delete QOS egress profile
 */
int
qos_map_egress_phb_remark_clear(int unit, int dscp_map)
{
    int rv;
    bcm_qos_map_t l3_eg_map;
    int dscp, tc, dp;

    
    if (is_device_or_above(unit,JERICHO2)) {
        bcm_qos_map_t_init(&l3_eg_map);
        l3_eg_map.opcode = l3_eg_opcode_id;

        /** Delete the maps for remarking */
        rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &l3_eg_map, l3_eg_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }    

    if (dscp_map) {
        if (is_device_or_above(unit,JERICHO2)) {
            for (dscp = 0; dscp < 64; dscp++) {
                for (dp = 0; dp < 2; dp++) {
                    bcm_qos_map_t_init(&l3_eg_map);
                    l3_eg_map.int_pri = dscp; 
                    l3_eg_map.color = dp; 

                    rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_REMARK, &l3_eg_map, l3_eg_opcode_id);
                    if (rv != BCM_E_NONE) {
                        printf("Error, set L3 PCP QoS mapping, bcm_qos_map_add, dscp=%d, dp=%d\n", dscp, dp);
                        return rv;
                    }
                }
            }
        } else {      
            for (dscp = 0; dscp < 64; dscp++) {
                bcm_qos_map_t_init(&l3_eg_map);
                l3_eg_map.dscp = dscp;
                rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_IPV4, &l3_eg_map, l3_eg_map_id);
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_qos_map_delete $rv \n");
                    return rv;
                }
            }
        }
    } else {
        for (tc=0; tc<8; tc++) {
            for (dp=0; dp<2; dp++) {
                bcm_qos_map_t_init(&l3_eg_map);
                l3_eg_map.int_pri = tc; 
                l3_eg_map.color = dp; 

                rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP, &l3_eg_map, l3_eg_map_id);    
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_qos_map_delete $rv \n");
                    return rv;
                }
            }
        }  
    }

    if (is_device_or_above(unit,JERICHO2)) {
        rv = bcm_qos_map_destroy(unit, l3_eg_opcode_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_qos_map_destroy $rv \n");
            return rv;
        }
    }

    rv = bcm_qos_map_destroy(unit, l3_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_qos_map_destroy $rv \n");
        return rv;
    }
    
    return rv;
}

/*add eve action on out_port in vsi
   update outer tag vid to out_vlan
   update outer tag priority by QoS map: egr_qos_id
   if it is for native link layer for JerichoPlus, QAX and above, 
   should create native AC and match it to tunnel*/
int vxlan_roo_eve_action(int unit, int out_port, int out_vlan, int vsi, int is_native)
{
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_action_set_t action;
    bcm_gport_t vlan_port_id;
    int is_adv_vt;
    bcm_port_match_info_t port_match_info;

    is_adv_vt = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0) || is_device_or_above(unit, JERICHO2);     

    rv = bcm_port_class_set(unit, out_port, bcmPortClassId, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set\n");
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2)) {
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION; 
        vlan_port.vsi = vsi;
        if (is_native) {
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
            vlan_port.match_class_id = 0; /* network domain, part of the key in ESEM lookup */
        } else {
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.port = out_port; 
        }

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
           printf("Error, bcm_vlan_port_create\n");
           return rv;
        }
        vlan_port_id = vlan_port.vlan_port_id;
    } else {
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;  
        if (is_device_or_above(unit,JERICHO_PLUS) && is_native){
            vlan_port.flags |= BCM_VLAN_PORT_NATIVE;
        }    
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
           printf("Error, bcm_vlan_port_create\n");
           return rv;
        }
        vlan_port_id = vlan_port.vlan_port_id;

        if (is_device_or_above(unit,JERICHO_PLUS) && is_native){        
            /*bind eve ac to tunnel port*/
            bcm_port_match_info_t_init(&port_match_info);

            port_match_info.flags = BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY; /* indicate that it's for native VE */
            port_match_info.match = BCM_PORT_MATCH_PORT_VPN; /* indicate that lookup fields are port and VPN */
            port_match_info.vpn = vsi;
            port_match_info.port = ip_tunnel_basic_info.encap_tunnel_id[0];

            rv = bcm_port_match_add(unit, vlan_port_id, &port_match_info);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_match_add \n ");
            }
        } else {
            bcm_port_match_info_t_init(&port_match_info);

            port_match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY; 
            port_match_info.match = BCM_PORT_MATCH_PORT_VLAN; /* indicate that lookup fields are port and VLAN */
            port_match_info.match_vlan = vsi;
            port_match_info.port = out_port;

            rv = bcm_port_match_add(unit, vlan_port_id, &port_match_info);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_match_add \n ");
            }
        }
    }

    /* set port translation info*/
    if (is_adv_vt) {
        rv = vlan_port_translation_set(unit, out_vlan, 0, vlan_port_id, g_eve_edit_utils.edit_profile, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set\n");
            return rv;
        }


        /* Create action IDs*/
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &g_eve_edit_utils.action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set translation action 1. outer action, set TPID 0x8100. */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionNone;
        action.dt_outer_pkt_prio = bcmVlanActionAdd;
        action.outer_tpid = 0x8100;
        action.priority = is_device_or_above(unit, JERICHO2) ? 0 : ip_tunnel_basic_info.egr_qos_id;
        rv = bcm_vlan_translate_action_id_set(unit,
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               g_eve_edit_utils.action_id,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }

        /* set action class */
        rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_default_translate_action_class_set\n");
            return rv;
        }
        /* bcm_qos_port_map_set don't support the remake profile configuration JR2 native */
        if (is_device_or_above(unit, JERICHO2) && is_native) {
            return rv;
        } else {
            /* in advanced vlan translation mode, The priority field isn't used, The QoS mapping is associated with the Out-LIF using bcm_qos_port_map_set(). */
            rv = bcm_qos_port_map_set(unit, vlan_port_id,-1,ip_tunnel_basic_info.egr_qos_id);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_qos_port_map_set\n");
                return rv;
            }
       }
    } else {
        /* Associate Out-AC to QOS profile ID, by egress vlan editing */
        bcm_vlan_action_set_t_init(&action);
        action.ut_outer = bcmVlanActionAdd;
        action.ut_outer_pkt_prio = bcmVlanActionAdd;
        action.new_outer_vlan = out_vlan;
        action.priority = is_device_or_above(unit, JERICHO2) ? 0 : ip_tunnel_basic_info.egr_qos_id;

        rv = bcm_vlan_translate_egress_action_add(unit, vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_egress_action_add\n");
            return rv;
        }
    }

    return rv;
}   

/*add eve action for access, overlay and native*/
int vxlan_roo_eve_action_set(int unit, int access_port, int provider_port)
{
    int rv;
    
    rv = vxlan_roo_eve_action(unit, access_port, ip_tunnel_basic_info.access_vlan, ip_tunnel_basic_info.access_eth_rif, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_roo_eve_action for access eve\n");
        return rv;
    }
    
    /* Due to ESEM access limitation, use the ARP+AC for the outer VLAN, no EVE configuration here */
    if (!is_device_or_above(unit,JERICHO2)) {
        if (is_device_or_above(unit,JERICHO_PLUS)) {
            rv = vxlan_roo_eve_action(unit, provider_port, ip_tunnel_basic_info.provider_vlan, ip_tunnel_basic_info.provider_eth_rif, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, vxlan_roo_eve_action for overlay eve\n");
                return rv;
            }
        } else {
            bcm_l2_egress_t l2_egress_overlay;

            /*JER don't support the EVE for overlay, update the outer_vid by bcm_l2_egress_create */
            bcm_l2_egress_t_init(&l2_egress_overlay);
            rv = bcm_l2_egress_get(unit, ip_tunnel_basic_info.encap_arp_id[0], &l2_egress_overlay);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_l2_egress_get\n");
                return rv;
            }
            l2_egress_overlay.flags = BCM_L2_EGRESS_WITH_ID | BCM_L2_EGRESS_REPLACE | BCM_L2_EGRESS_OUTER_VLAN_REPLACE;
            l2_egress_overlay.encap_id = ip_tunnel_basic_info.encap_arp_id[0];
            l2_egress_overlay.outer_vlan = ip_tunnel_basic_info.provider_vlan;             
            rv = bcm_l2_egress_create(unit, &l2_egress_overlay);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_l2_egress_create\n");
                return rv;
            }
        }
    } else {        
        int arp_outlif;
        bcm_gport_t arp_ac_gport;
        
        arp_outlif = BCM_L3_ITF_VAL_GET(ip_tunnel_basic_info.encap_arp_id[0]);
        BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_ac_gport, arp_outlif);
        BCM_GPORT_VLAN_PORT_ID_SET(arp_ac_gport, arp_ac_gport);
        
        rv = vlan_translate_ive_eve_translation_set_with_pri_action(unit, arp_ac_gport,                         /* lif  */
                                                          0x8100,                               /* outer_tpid */
                                                          0x9100,                               /* inner_tpid */
                                                          bcmVlanTpidActionModify,              /* outer_tpid_action */
                                                          bcmVlanTpidActionNone,                /* inner_tpid_action */  
                                                          bcmVlanActionArpVlanTranslateAdd,     /* outer_action */
                                                          bcmVlanActionNone,                    /* inner_action */
                                                          bcmVlanActionAdd,                     /* outer_pri_action */
                                                          bcmVlanActionNone,                    /* inner_pri_action */
                                                          ip_tunnel_basic_info.provider_vlan,   /* new_outer_vid */
                                                          0,                                    /* new_inner_vid */                                                          
                                                          3,                                    /* vlan_edit_profile */
                                                          0,                                    /* tag_format - must be untag! */
                                                          FALSE                                 /* is_ive */
                                                          );
        if (rv != BCM_E_NONE) {
            printf("Error, vxlan_roo_eve_action for overlay eve\n");
            return rv;
        }
    }        
    
    rv = vxlan_roo_eve_action(unit,provider_port,ip_tunnel_basic_info.provider_native_vlan,ip_tunnel_basic_info.provider_native_eth_rif,1);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_roo_eve_action for native eve\n");
        return rv;
    }

    return rv;
}   

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * provider_port - port where DC Fabric router is connected to.
 */
int vxlan_roo_example(
    int unit,
    int is_ipv6,
    int access_port,
    int provider_port,
    int add_routing_table_entry)
{

    int rv;
    int tunnel_idx = 0;
    int native_ll_idx = 0;
    int native_ll_num = 1;
    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;
    
    vxlan_roo_init(unit, is_ipv6, access_port, provider_port);
    
    /* 
     * Build L2 VPN
     */
    rv = sand_utils_vxlan_open_vpn(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc, cint_vxlan_basic_info.vni);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), in sand_utils_vxlan_open_vpn, vpn=%d, vdc=%d \n", rv, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc);
        return rv;
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
    /* Config VRRP my mac */
    rv = vxlan_my_mac_vrrp(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_my_mac_vrrp \n", rv);
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

    /* if ecmp is enabled,
       we'll add an additional FEC, which point to tunnel-outlif.
       Tunnel-outlif point to ARP
       so we create an additional ARP
    */
    if (cint_vxlan_basic_info.second_level_ecmp_enable) {
         rv = ip_tunnel_encap_create_arp(unit, tunnel_idx + 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ip_tunnel_encap_create_arp\n");
            return rv;
        }
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

    /* if ecmp is enabled,
       we'll add an additional FEC, which point to tunnel-outlif.
       so we create an additional tunnel outlif
    */
    if (cint_vxlan_basic_info.second_level_ecmp_enable) {
        /*
         * Create the tunnel initiator
         */
        rv = ip_tunnel_encap_create(unit, tunnel_idx + 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in ip_tunnel_encap_create \n", rv);
            return rv;
        }
    }

    /*
     * Configure fec entry
     */
    rv = ip_tunnel_create_fec(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ip_tunnel_create_fec\n", rv);
        return rv;
    }

    /* 
     * If ECMP is enabled, we 
     * create an additional FEC 
     */
    if (cint_vxlan_basic_info.second_level_ecmp_enable) {
        rv = ip_tunnel_encap_create_fec(unit, tunnel_idx + 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, ip_tunnel_encap_create_fec\n");
            return rv;
        }
    }

    /*
     * Create the tunnel initiator ECMP
     */
    rv = vxlan_second_level_ecmp_add(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_ecmp_add \n", rv);
        return rv;
    }  

    /* 
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_vxlan_port_add(unit, ip_tunnel_basic_info.provider_port, tunnel_idx, cint_vxlan_basic_info.vxlan_egress_if_is_ecmp);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), vxlan_vxlan_port_add \n", rv);
        return rv;
    }

    /* ecmp point to 2 tunnel FECs.
       Each tunnel FECS point to tunnel outlif
       one tunnel outlif was configured by vxlan port add (orientation)
       the second tunnel outlif needs to be configured as well. */
    if (cint_vxlan_basic_info.second_level_ecmp_enable) {
        rv = vxlan_outlif_update_orientation(unit, tunnel_idx+1);
        if (rv != BCM_E_NONE) 
        {
            printf("Error(%d), vxlan_outlif_update_network_group \n", rv);
            return rv;
        }
    }

    /*
        * The 1st native link layer is used for ip host.
        * The 2nd native link layer is used for ip route.
        */
    if (add_routing_table_entry) {
        native_ll_num = 2;
        cint_vxlan_basic_info.add_routing_table_entry = add_routing_table_entry;
    }
    for (native_ll_idx = 0; native_ll_idx < native_ll_num; native_ll_idx++) {
        /*
         * Configure native arp entry
         */
        rv = ip_tunnel_encap_create_arp_native(unit, native_ll_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in ip_tunnel_encap_create_fec_native\n", rv);
            return rv;
        }

        /*
         * Configure native fec entry
         */
        rv = ip_tunnel_encap_create_fec_native(unit, native_ll_idx);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in ip_tunnel_encap_create_fec_native\n", rv);
            return rv;
        }
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
    
    rv = vxlan_fwd_l3(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_fwd_l3 \n", rv);
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


