/* $Id: cint_ip_route_scenarios.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



/*
 * Test Scenario
 *   test functionality of IPv6 route with different prefix length
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * ipv6_route_prefix(0,200,201);
 * exit;
 *
 * step 1:
 * tx 1 psrc=200 data=0x000a00000b001100000000118100006486dd608000100014060a12345678000000000000000087654321a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a53344556600a1a2a300b1b2b3500f01f56e2d0000
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00000022000a00000b008100006486dd608000100014060012345678000000000000000087654321a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a53344556600a1a2a300b1b2b3500f01f56e2d000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * step 2:
 * cint
 * ipv6_route_prefix_entry_delete(0,prefix_length);
 * exit;
 *
 * Repeat step 1 and 2 with different prefix_length.
 * the value of prefix_length starting with 124, and decreasing by 4 one time, until prefix length is 4.
 */

/*
 * Test Scenario
 *   test functionality of Raw data redirect
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 *
 * dnx_basic_example_raw_redirect(0,200,201,202,10,0xA711,0xA811,0x1484);
 * exit;
 *
 * step 1:
 * tx 1 psrc=200 data=0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f
 *
 *    Received packets on unit 0 should be:
 *    Source port: 200, Destination port: 203
 *    Data: 0x00000000cd1d00123456789a81000064080045000035000000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f
 */

/* debug prints */
int verbose = 1;

int ip_route_fec_with_id = 0;
int fall2bridge_ive_enable = 0;

/* Variable to store the returned entry handle*/
bcm_field_entry_t entry_id_created;
/*
   check if FEC entry was accessed by traffic lookup
   e.g. fec_accessed(0,0x7fffff03,0,0);
*/
int fec_accessed(int unit,bcm_if_t intf, uint8 clear_accessed ) {
  int rc = BCM_E_NONE;
  bcm_l3_egress_t l3eg;
  bcm_l3_egress_t_init(&l3eg);

  if (clear_accessed) {
      l3eg.flags |= BCM_L3_HIT_CLEAR;
  }

  rc = bcm_l3_egress_get(unit,intf,&l3eg);
  if (rc != BCM_E_NONE) {
    printf ("fec_get failed: %x \n", rc);
  }
  if (l3eg.flags & BCM_L3_HIT) {
      printf ("Accessed \n");
  }
  else{
      printf ("Not Accessed \n");
  }

  return rc;
}

int
ipv6_route_prefix(
    int unit,
    int in_port,
    int out_port)
{
    int prefix, rv = 0;
    int vrf = 100;
    bcm_ip6_t route = { 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5 };
    int intf;              /* ETH-RIF */
    int fec;
    int vlan;
    int encap_id = 0;    /* ARP-Link-layer */
    bcm_gport_t gport = 0;
    l3_ingress_intf ingress_itf;

    bcm_mac_t my_mac = { 0x00, 0x0a, 0x00, 0x00, 0x0b, 0x00 };
    bcm_mac_t next_hop_mac = { 0x22, 0x00, 0x00, 0x00, 0x00, 0x22 };
    bcm_l3_route_t l3route;

    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;
    char *proc_name;
    proc_name = "ipv6_route_prefix";
    uint32 fwd_only_flags = 0;

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        fwd_only_flags = BCM_L3_FLAGS2_FWD_ONLY;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    intf = vrf;
    rv = in_port_intf_set(unit, in_port, intf);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("%s(): Error, in_port_intf_set in_port = %d, intf_in = %d, err_id = %d\n", proc_name, in_port, intf,
               rv);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("%s(): Error, out_port_intf_set out_port = %d, err_id = %d\n", proc_name, out_port, rv);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf, my_mac);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, intf, rv);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    vlan = vrf;
    ingress_itf.vrf = vrf;
    ingress_itf.intf_id = vlan;
    rv = intf_ingress_rif_set(unit, &ingress_itf);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, vlan, vrf, rv);
        return rv;
    }


    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    l3route.l3a_flags2 = fwd_only_flags;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    l3route.l3a_vrf = vrf;

    for(prefix = 124; prefix >= 0; prefix -= 4) {
        ipv6_create_mask(l3route.l3a_ip6_mask, prefix);
        /*
         * 5. Create ARP and set its properties
         */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id, 0, 0, next_hop_mac, vlan);
        rv = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, sand_utils_l3_arp_create: encap_id = %d, vsi = %d, err_id = %d\n", proc_name,
                   encap_id, vlan, rv);

        }

        /*
         * 6. Create FEC and set its properties
         */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        BCM_GPORT_LOCAL_SET(l3_fec.destination, out_port);
        l3_fec.tunnel_gport = vlan;
        l3_fec.tunnel_gport2 = encap_id;
        l3_fec.fec_id = fec;
        rv = sand_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, sand_utils_l3_fec_create: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
                 proc_name, fec, vlan, encap_id, out_port, rv);
            return rv;
        }


        l3route.l3a_intf = l3_fec.l3eg_fec_id;
        rv = bcm_l3_route_add(unit, &l3route);
        if(rv != BCM_E_NONE) {
             printf("%s(): Error, bcm_l3_route_add Failed, prefix %d\n",  proc_name,prefix);
             return rv;
        }
        encap_id = 0;
    }

    return BCM_E_NONE;
}

int
ipv6_route_prefix_entry_delete(
    int unit,
    int prefix)
{
    int rv = 0;
    int vrf = 100;
    bcm_ip6_t route = { 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5 };
    bcm_l3_route_t l3route;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    ipv6_create_mask(l3route.l3a_ip6_mask, prefix);
    l3route.l3a_vrf = vrf;

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        l3route.l3a_flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }

    rv = bcm_l3_route_delete(unit, &l3route);
    if(rv != BCM_E_NONE) {
         printf("Error, bcm_l3_route_delete Failed, prefix %d\n", prefix);
         return rv;
    }

    return BCM_E_NONE;
}

/*
 * Test Scenario
 *   test functionality of public/private IPv4 UC KAPS
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * public_private_config(0,200,201,202,203);
 * exit;
 *
 *
 * tx 1 psrc=201 data=0xa000000000e610000000002b81000064080045000040000000008000a1f90a042b0f2036437cc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00000054a000000000e4810000c8080045000040000000007f00a2f90a042b0f2036437cc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 *
 * tx 1 psrc=200 data=0xa000000000e410000000002b810000c8080045000040000000008000a1f92036437c0a042b0fc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000002ba000000000e681000064080045000040000000007f00a2f92036437c0a042b0fc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */

int
public_private_config(
    int unit,
    int port1,
    int port2,
    int port3,
    int port4)
{
    int rc, rv;
    int i;
    int egress_port;
    const int num_ports = 2;

    bcm_mac_t mac_l3_ingress;
    bcm_mac_t mac_l3_ingress100 = {0xa0, 0x0, 0x0, 0x0, 0x0, 0xe6};
    bcm_mac_t mac_l3_ingress200 = {0xa0, 0x0, 0x0, 0x0, 0x0, 0xe4};

    bcm_mac_t mac_l3_egress;
    bcm_mac_t mac_l3_egress100 = {0x10, 0x0, 0x0, 0x0, 0x0, 0x2b};
    bcm_mac_t mac_l3_egress200 = {0xb0, 0x0, 0x0, 0x0, 0x0, 0x54};

    int host[2] = {0x2036437C, 0xa042B0F};          /* {"32.54.67.124", "10.4.43.15"}; */
    int route[2];
    bcm_vlan_t vid[2] = {100, 200};
    bcm_vlan_t vlan;

    int fec[2] = {0x0};
    int encap_id[2] = {0x2384, 0x2385};
    uint32 flags2 = 0;

    bcm_l3_intf_t l3if[num_ports];
    int ingress_intf[num_ports];
    int egress_intf[num_ports];
    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;

    /* create_l3_egress_s l3eg; */

    int egress_port_list_one_device[num_ports] = {port1, port2};

    for (i=0; i<num_ports; i++) {
        egress_port=egress_port_list_one_device[i];

        if (i == 1) {
            mac_l3_ingress = mac_l3_ingress100;
            mac_l3_egress = mac_l3_egress100;
            vlan = 100;
        } else {
            mac_l3_ingress = mac_l3_ingress200;
            mac_l3_egress = mac_l3_egress200;
            vlan = 200;
        }

        /*
         * 1. Set In-Port to In ETh-RIF
         */
        rv = in_port_intf_set(unit, egress_port, vlan);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set intf_in\n");
            return rv;
        }

        /*
         * 2. Set Out-Port default properties
         */
        rv = out_port_set(unit, egress_port);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, out_port_intf_set intf_out\n");
            return rv;
        }

        bcm_l3_intf_t_init(&(l3if[i]));
        l3if[i].l3a_flags = BCM_L3_WITH_ID;
        sal_memcpy(l3if[i].l3a_mac_addr, mac_l3_ingress, 6);
        l3if[i].l3a_vrf = vlan;
        l3if[i].l3a_intf_id = l3if[i].l3a_vid = vlan;
        rc = bcm_l3_intf_create(unit, l3if[i]);
        if (rc != BCM_E_NONE) {
            printf("Error, bcm_l3_intf_create");
        }
        ingress_intf[i] = vlan;

        if(i == 1) {

            bcm_l3_ingress_t l3_ing_if;
            bcm_l3_ingress_t_init(&l3_ing_if);

            l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
            l3_ing_if.vrf = l3if[i].l3a_vrf;
            l3_ing_if.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;

            rc = bcm_l3_ingress_create(unit, l3_ing_if, ingress_intf[i]);
            if (rc != BCM_E_NONE) {
                printf("Error, bcm_l3_ingress_create\n");
                return rc;
            }
        }


        /*
         * 5. Create ARP and set its properties
         */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[i], 0, 0, mac_l3_egress, vlan);
        rv = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_arp_create: encap_id = %d, vsi = %d, err_id = %d\n",
                   encap_id[i], vlan, rv);
        }
        encap_id[i] = l3_arp.l3eg_arp_id;

        /*
         * 6. Create FEC and set its properties
         */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        BCM_GPORT_LOCAL_SET(l3_fec.destination, egress_port);
        l3_fec.tunnel_gport = ingress_intf[i];
        l3_fec.tunnel_gport2 = encap_id[i];
        l3_fec.fec_id = fec[i];
        rv = sand_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_fec_create: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
                 fec[i], ingress_intf[i], encap_id[i], egress_port, rv);
            return rv;
        }
        egress_intf[i] = l3_fec.l3eg_fec_id;

    }

    int mask = 0xFFFFFFFF << 8;
    for (i=0; i<num_ports; i++) {
        bcm_l3_route_t l3rt;
        bcm_l3_route_t_init(&l3rt);

        route[i] = host[i] & mask;

        l3rt.l3a_subnet = route[i];
        l3rt.l3a_ip_mask = mask;
        l3rt.l3a_vrf = vid[i];
        if(i == 0) {
            l3rt.l3a_vrf = 0;
        }
        l3rt.l3a_intf = egress_intf[i];

        rc = bcm_l3_route_add(unit, &l3rt);
        if (rc != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %d \n", rc);
        }
    }

    return 0;
}

int public_private_ipv4_route_remove(int unit, int index) {

    int rv = 0;

    bcm_l3_route_t l3_route;
    int host[2] = { 0x2036437c, 0x0a042b0f };
    int vrf[2] = { 0, 200 };
    int mask = 0xffffff00;

    if(index < 0 || index > 2) {
        printf("Error removing unexisting route\n");
        return -1;
    }

    bcm_l3_route_t_init(&l3_route);
    l3_route.l3a_subnet = host[index];
    l3_route.l3a_ip_mask = mask;
    l3_route.l3a_vrf = vrf[index];
    rv = bcm_l3_route_delete(unit, &l3_route);
    if(rv != BCM_E_NONE) {
        printf("Error, bcm_l3_route_delete\n");
    }

    return rv;
}

/*
 * Test Scenario
 *   test ipv4 default route
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * basic_example(0,200,201);
 * exit;
 *
 * cint
 * default_route_config(0,200,201);
 * exit;
 *
 * tx 1 psrc=200 data=0x000c00020000000067660b308100000f08004508002e0000400040066a32102040807ffffff03344556600a1a2a300b1b2b3500f01f530250000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1d000c000200008100006508004508002e000040003f066b32102040807ffffff03344556600a1a2a300b1b2b3500f01f530250000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */

/*
 * set default Router destination for given VRF
 * - vrf: max value depeneds on soc-property in arad.soc
 * - is_mc: wether this set the default destination for UC packet or MC packet.
 * - intf: egress object created using create_l3_egress
 */
int set_default_route(int unit, int vrf, uint8 is_mc, int intf) {
  int rc;
  bcm_l3_route_t l3rt;

  bcm_l3_route_t_init(l3rt);

  l3rt.l3a_flags = 0;

  if(is_mc) {
      l3rt.l3a_flags |= BCM_L3_IPMC;
  }

  /* to indicate it's default route, set subnet to zero */
  l3rt.l3a_subnet = 0;
  l3rt.l3a_ip_mask = 0;

  l3rt.l3a_vrf = vrf;
  rc = bcm_l3_route_get(unit, &l3rt);
  if (rc == BCM_E_NONE)
  {
      printf("Providing replace flag to the route structure. \n");
      l3rt.l3a_flags |= BCM_L3_REPLACE;
  }
  l3rt.l3a_intf = intf;
  rc = bcm_l3_route_add(unit, l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %x \n", rc);
  }

  return rc;
}

/*
 * set default route to send to out-port.
 * out-port:  egress port for default route.
 */
int
default_route_config(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int ing_intf_in;
    int ing_intf_out;
    int in_vlan = 15;
    int out_vlan = 101;
    int vrf = 0;
    int fec = 40971;
    int encap_id = 0;           /* ARP-Link-layer */
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac */
    bcm_gport_t gport;
    sand_utils_l3_port_s l3_port_structure;

    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;
    bcm_if_t encoded_fec;


    /*
     * 1. Create ETH-RIF and set its properties
     */
    sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, in_port, in_vlan);
    rv = sand_utils_l3_port_set(unit, &l3_port_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_port_set l3_port_structure\n");
        return rv;
    }
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, in_vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, mac_address, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf_in = eth_rif_structure.l3_rif;

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, out_vlan, 0, 0, mac_address, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf_out = eth_rif_structure.l3_rif;

    /*
     * 2. Create ARP and set its properties
     */
    sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id, next_hop_mac, out_vlan);
    rv = sand_utils_l3_arp_create(unit, &l3_arp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }
    encap_id = l3_arp.l3eg_arp_id;

    /*
     * 3. Create FEC and set its properties
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
    BCM_GPORT_LOCAL_SET(l3_fec.destination, out_port);
    l3_fec.tunnel_gport = ing_intf_out;
    l3_fec.tunnel_gport2 = encap_id;
    l3_fec.fec_id = fec;
    rv = sand_utils_l3_fec_create(unit, &l3_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    encoded_fec = l3_fec.l3eg_fec_id;

    /* set default route to point to created FEC */
    rv = set_default_route(unit, vrf, 0/*is-MC*/, encoded_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, set_default_route\n");
    }
    printf("default route ---> egress-object=0x%08x, port=%d, \n", encoded_fec, out_port);
    return rv;
}

/*
 * Test Scenario
 *   test functionality of IPvr host with no FEC
 *   host lookup returns: <dest-port, out-RIF, MAC (pointed by outlif) >
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * ip_host_no_fec_example(0,200,201);
 * exit;
 *
 * tx 1 psrc=200 data=0x000c000200000000c79d26678100000f08004508002e000040004006e121102040800a00ff003344556600a1a2a300b1b2b3500f01f5a7140000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00030000000c000200008100006408004508002e000040003f06e221102040800a00ff003344556600a1a2a300b1b2b3500f01f5a7140000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */

int
ip_host_no_fec_example (
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int ing_intf_in;
    int ing_intf_out;
    int fec = 0;
    int encap_id = 0;
    int flags = 0;
    int in_vlan = 15;
    int out_vlan = 100;
    int vrf = 15;
    int host = 0x0a00ff00;
    bcm_vlan_control_vlan_t control_vlan;
    sand_utils_l3_eth_rif_s eth_rif_structure;
    bcm_mac_t my_mac_address_in   = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};
    bcm_mac_t my_mac_address_out  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00};
    bcm_l3_host_t l3host;

    /*
     * 1. create ingress router interface
     */
    rv = bcm_vlan_create(unit, in_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, create vlan=%d, in unit %d \n", in_vlan, unit);
    }

    /* Set VLAN with MC */
    rv = bcm_vlan_control_vlan_get(unit, in_vlan, &control_vlan);
    if (rv != BCM_E_NONE) {
        printf("fail get control vlan(%d)\n", in_vlan);
        return rv;
    }
    control_vlan.unknown_unicast_group =
        control_vlan.unknown_multicast_group =
        control_vlan.broadcast_group = 0x1;
    rv = bcm_vlan_control_vlan_set(unit, in_vlan, control_vlan);
    if (rv != BCM_E_NONE) {
        printf("fail set control vlan(%d)\n", in_vlan);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
      return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, in_vlan, 0, 0, my_mac_address_in, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf_in = eth_rif_structure.l3_rif;

    /*
     * 2. create egress router interface
     */
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, create vlan=%d, in unit %d \n", out_vlan, unit);
    }

    /* Set VLAN with MC */
    rv = bcm_vlan_control_vlan_get(unit, out_vlan, &control_vlan);
    if (rv != BCM_E_NONE) {
        printf("fail get control vlan(%d)\n", out_vlan);
        return rv;
    }
    control_vlan.unknown_unicast_group =
        control_vlan.unknown_multicast_group =
        control_vlan.broadcast_group = 0x1;
    rv = bcm_vlan_control_vlan_set(unit, out_vlan, control_vlan);
    if (rv != BCM_E_NONE) {
        printf("fail set control vlan(%d)\n", out_vlan);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
      return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, out_vlan, 0, 0, my_mac_address_out, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf_out = eth_rif_structure.l3_rif;

    /*
     * 3. use egress to set MAC address and out_vlan, FEC is not created
     */

    rv = l3__egress_only_encap__create(unit, 0, &encap_id, next_hop_mac, out_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    if(verbose >= 1) {
        printf("encap-id = %08x", encap_id);
        printf("outRIF = 0x%08x out-port =%d", ing_intf_out, fec);
    }

    /*
     * 5. add host entry
     *      for DPP: host entry with outlif(outRIF + MAC) + port
     *               VSI is taken from encap-id
     *      for DNX: host entry with outRIF + outlif (MAC) + port
     */

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = ing_intf_out;             /* relevant only for DNX: OUTRIF */
    l3host.l3a_modid = 0;
    l3host.l3a_port_tgid = out_port; /* egress port to send packet to */
    l3host.encap_id = encap_id;

    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", rv);
        printf("Error, create egress object, in_port=%d, \n", in_port);
    }
    print_host("add entry ", host,vrf);
    printf("---> egress-encap=0x%08x, outRIF = 0x%08x, port=%d, \n", encap_id, ing_intf_out, out_port);

    return rv;
}

/* Purpose: pmf parse the raw payload reult from the LPM (KAPS).
 *      if the result from the kaps= lpm_payload (0x12345)
 *      then forward the packet to port 13
 *
 * Usage:
 * when adding an entry to the LPM with api: "bcm_l3_route_add()"
 * use the flag "BCM_L3_FLAGS2_RAW_ENTRY"
 * see example in cint_ip_route; basic_example() with "lpm_raw_data_enabled=1"
 *
 * Add PMF rule to parse the result from the KAPS
 */

int lpm_raw_data_enabled =0;         /* use raw data payload in KAPs LPM */
int lpm_payload = 0x1234;            /* raw payload in Kaps*/
int lpm_raw_new_dst_port;
int large_direct_lu_key_length=0;    /*configurable large direct lu key length (14/15/16/17)*/

int
dpp_kaps_raw_data_redirect (
    int unit,
    int kaps_result,
    int redir_port)
{
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t action_entry=1;
    int sys_gport;
    int result = 0;
    bcm_field_entry_t new_entry = 0;
    int group_id;
    int key_val;
    uint32 param0, param1;
    bcm_gport_t dst_gport;
    if (!BCM_GPORT_IS_TRUNK(redir_port)) {
        BCM_GPORT_LOCAL_SET(dst_gport, redir_port);
    } else {
        dst_gport = redir_port;
    }
    bcm_field_data_qualifier_t data_qual;

    if (large_direct_lu_key_length)
    {
        result = bcm_field_control_set(unit, bcmFieldControlLargeDirectLuKeyLength, large_direct_lu_key_length);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_control_set with Err %x\n", result);
            return result;
        }

        bcm_field_data_qualifier_t_init(&data_qual);
        data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
        data_qual.qualifier = bcmFieldQualifyL3DestRouteValue;
        data_qual.length = large_direct_lu_key_length;
        data_qual.offset = 0;

        result = bcm_field_data_qualifier_create(unit, &data_qual);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_data_qualifier_create with Err %x\n", result);
            return result;
        }
    }

    int presel_id = 0;
    int presel_flags = 0;
    bcm_field_stage_t stage;

    bcm_field_presel_set_t presel_set;
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    /*Define preselector*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            return result;
        } else {
            printf("bcm_field_presel_create_id: %x\n", presel_id);
        }
    } else {
        result = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            return result;
        } else {
            printf("bcm_field_presel_create_id: %x\n", presel_id);
        }
    }

    BCM_FIELD_PRESEL_INIT(presel_set);
    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);

    stage = bcmFieldStageIngress;
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);

    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);
    if (!large_direct_lu_key_length) {
        grp.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    } else {
        grp.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_LARGE | BCM_FIELD_GROUP_CREATE_WITH_ID;
        grp.mode = bcmFieldGroupModeDirect;
        grp.group = 22 + large_direct_lu_key_length;
    }

    grp.preselset = presel_set;

    /*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);

    if (!large_direct_lu_key_length) {
        if(is_kbp) {
            BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue0);
        } else {
            BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyL3DestRouteValue);
        }
    } else {
        result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
        if (BCM_E_NONE != result) {
            printf("bcm_field_qset_data_qualifier_add %x\n", result);
            return result;
        }
    }

    /* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n", result);
        return result;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    /* Attached the action to the field group */
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n", result);
        return result;
    }

    /*create an entry*/
    result = bcm_field_entry_create_id(unit, grp.group, action_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    if (!large_direct_lu_key_length) {
        uint64 data;
        uint64 mask;
        COMPILER_64_SET(data, 0x00000000, kaps_result);
        COMPILER_64_SET(mask, 0x00000000, 0xfffff);
        if(is_kbp) {
            result = bcm_field_qualify_ExternalValue0(unit, action_entry, data, mask);
        } else {
            result = bcm_field_qualify_L3DestRouteValue(unit, action_entry, data, mask);
        }
        if (BCM_E_NONE != result) {
            return result;
        }
    } else {
        int byte; /* iterator */
        int kaps_mask = (1 << large_direct_lu_key_length) - 1; /* large_direct_lu_key_length bits set */
        int size = ((large_direct_lu_key_length - 1) / 8) + 1; /* bytes of data / mask */
        uint8 data1[size], mask1[size];
        for(byte = 0; byte < size; byte++) {
            data1[byte] = (kaps_result >> ((size - byte - 1) * 8)) % 256;
            mask1[byte] = (kaps_mask >> ((size - byte - 1) * 8)) % 256;
        }

        result = bcm_field_qualify_data(unit, action_entry, data_qual.qual_id, &data1, &mask1, size);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_data\n");
            return result;
        }
    }

    result = bcm_field_action_add(unit, action_entry, bcmFieldActionRedirect, 0, dst_gport);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    if (soc_property_get(unit, spn_PMF_KAPS_MGMT_ADVANCED_MODE, FALSE)) {
        result = bcm_field_entry_install_and_handler_update(unit, action_entry, &new_entry);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_entry_install_and_handler_update\n");
            return result;
        }

        if (!(new_entry & BCM_FIELD_ENTRY_LARGE_DIRECT_LOOKUP)) {
            printf("The working mode is not advanced mode.\n");
            return BCM_E_FAIL;
        }

        BCM_FIELD_LARGE_DIRECT_ENTRY_ID_UNPACK(new_entry, group_id, key_val);

        if (group_id != grp.group) {
            printf("The expected group ID:%d, but set value:%d.\n", grp.group, group_id);
            return BCM_E_FAIL;
        }

        if (key_val != kaps_result) {
            printf("The expected key value: %d, but set value:%d.\n", kaps_result, key_val);
            return BCM_E_FAIL;
        }

        result = bcm_field_action_get(unit, new_entry, bcmFieldActionRedirect, &param0, &param1);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_action_get\n");
            return result;
        }

        uint8 data2[2], mask2[2];
        uint16 length;
        result = bcm_field_qualify_data_get(unit, new_entry, data_qual.qual_id, 2, &data2, &mask2, &length);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_data_get\n");
            return result;
        }

        if (data2[0] != ((kaps_result >> 8) & 0xFF)) {
            printf("The expected key value: %d, but set value:%d.\n", data2[0], ((kaps_result >> 8) & 0xFF));
            return BCM_E_FAIL;
        }

        if (data2[1] != (kaps_result & 0xFF)) {
            printf("The expected key value: %d, but set value:%d.\n", data2[1], (kaps_result & 0xFF));
            return BCM_E_FAIL;
        }
    } else {
        result = bcm_field_group_install(unit, grp.group);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_group_install\n");
            return result;
        }
    }

    return result;
}


/*
 * Test Scenario
 *   test ipv4 route on multi-device
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * basic_example_multi_units(units_ids,1,1811939529,1811939530);
 * exit;
 *
 * tx 1 psrc=201 data=0x000c000200000000070001008100000f080045000035000000008000b1c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1d000c0002000081000064080045000035000000007f00b2c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 */
int L3_uc_rpf_mode = bcmL3IngressUrpfLoose;
int fec_id_used = 0;
int default_mtu = 0;
int default_mtu_forwarding = 0;

int
basic_example_multi_units (
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport)
{
    int rv;
    int i, unit;
    int ing_intf_in;
    int ing_intf_out;
    int fec[2] = {0, 0};
    int flags = 0;
    int flags2 = 0;
    int in_vlan = 15;
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0x2002, 0x2004}; /* EncapID use the valid global Out-LIF for both JR and JR2 */
    int open_vlan = 1;
    int route;
    int mask;
    int result = 0;
    bcm_mac_t mac_address  = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  /* my-MAC */
    bcm_mac_t next_hop_mac  = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = { 0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac2 */

    bcm_vlan_control_vlan_t control_vlan;
    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;
    sand_utils_l3_host_ipv4_s host_ipv4_structure;
    sand_utils_l3_route_ipv4_s route_ipv4_structure;
    int sysport_is_local = 0;
    int local_port = 0;
    bcm_gport_t dst_gport;

    if (lpm_raw_data_enabled) {
        vrf = 1;
        fec[1] = lpm_payload;
    }

    /*
     * For JR1, only one global MSBs My-MAC was supported. bcm_l2_station_add API is used to assign a global MSBs my-MAC to a VSI.
     * For JR2, there is no need to call bcm_l2_station_add. VSI global MSBs my-MAC assignment is managed internally using the API bcm_l3_intf_create.
     */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if (is_device_or_above(unit, JERICHO2)){
            continue;
        }

        int station_id;
        bcm_l2_station_t station;
        bcm_l2_station_t_init(&station);
        /* set my-Mac global MSB */
        station.flags = 0;
        sal_memcpy(station.dst_mac, mac_address, 6);
        station.src_port_mask = 0; /* port is not valid */
        station.vlan_mask = 0; /* vsi is not valid */
        station.dst_mac_mask[0] = station.dst_mac_mask[1] = station.dst_mac_mask[2] =
        station.dst_mac_mask[3] = station.dst_mac_mask[4] = station.dst_mac_mask[5] = 0xff; /* dst_mac my-Mac MSB mask is -1 */

        rv = bcm_l2_station_add(unit, &station_id, &station);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l2_station_add\n");
            return rv;
        }
    }

    /*** create ingress router interface ***/
    flags = (vrf == 0) ? BCM_L3_INGRESS_GLOBAL_ROUTE : 0;
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = bcm_vlan_create(unit, in_vlan);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, create vlan=%d, in unit %d \n", in_vlan, unit);
        }

        /* Set VLAN with MC */
        rv = bcm_vlan_control_vlan_get(unit, in_vlan, &control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail get control vlan(%d)\n", in_vlan);
            return rv;
        }
        control_vlan.unknown_unicast_group =
            control_vlan.unknown_multicast_group =
            control_vlan.broadcast_group = 0x1;
        rv = bcm_vlan_control_vlan_set(unit, in_vlan, control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail set control vlan(%d)\n", in_vlan);
            return rv;
        }
        sysport_is_local = 0;
        rv = system_port_is_local(unit, in_sysport, &local_port, &sysport_is_local);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sysport_is_local().\n");
            return rv;
        }
        if(sysport_is_local == 1) {
            rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
            if (!BCM_GPORT_IS_TRUNK(in_sysport)) {
                if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                    printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
                    return rv;
                }
            }
        }
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, in_vlan, 0, flags, mac_address, vrf);
        eth_rif_structure.urpf_mode = L3_uc_rpf_mode;
        eth_rif_structure.mtu_valid  = 1;
        eth_rif_structure.mtu  = default_mtu;
        if (!is_device_or_above(unit, JERICHO2)) {
            eth_rif_structure.mtu_forwarding = default_mtu_forwarding;
        }
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            return rv;
        }
        ing_intf_in = eth_rif_structure.l3_rif;
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = bcm_vlan_create(unit, out_vlan);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, create vlan=%d, in unit %d \n", out_vlan, unit);
        }

        /* Set VLAN with MC */
        rv = bcm_vlan_control_vlan_get(unit, out_vlan, &control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail get control vlan(%d)\n", out_vlan);
            return rv;
        }
        control_vlan.unknown_unicast_group =
            control_vlan.unknown_multicast_group =
            control_vlan.broadcast_group = 0x1;
        rv = bcm_vlan_control_vlan_set(unit, out_vlan, control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail set control vlan(%d)\n", out_vlan);
            return rv;
        }

        sysport_is_local = 0;
        rv = system_port_is_local(unit, out_sysport, &local_port, &sysport_is_local);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sysport_is_local().\n");
            return rv;
        }
        if(sysport_is_local == 1) {
            rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
            if (!BCM_GPORT_IS_TRUNK(out_sysport)) {
                if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                    printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
                    return rv;
                }
            }
        }
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, out_vlan, 0, flags, mac_address, vrf);
        eth_rif_structure.urpf_mode = L3_uc_rpf_mode;
        eth_rif_structure.mtu_valid  = 1;
        eth_rif_structure.mtu  = default_mtu;
        if (!is_device_or_above(unit, JERICHO2)) {
            eth_rif_structure.mtu_forwarding = default_mtu_forwarding;
        }
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            return rv;
        }
        ing_intf_out = eth_rif_structure.l3_rif;
    }

    /*** create egress object 1 ***/
    /* out_sysport unit is already first */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        /* Create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[0], 0, 0, next_hop_mac, out_vlan);
        rv = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rv;
        }
        encap_id[0] = l3_arp.l3eg_arp_id;

        /* Create FEC and set its properties */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        l3_fec.destination = out_sysport;
        l3_fec.tunnel_gport = ing_intf_out;
        l3_fec.tunnel_gport2 = encap_id[0];
        l3_fec.allocation_flags = 0;
        if (ip_route_fec_with_id != 0) {
            l3_fec.allocation_flags |= BCM_L3_WITH_ID;
            fec[0] = ip_route_fec_with_id;
        }
        l3_fec.fec_id = fec[0];
        rv = sand_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }
        fec[0] = l3_fec.l3eg_fec_id;

        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
        }
    }

    if (!lpm_raw_data_enabled) {
        /*** add host point to FEC ***/
        /*In JR2, if public entries were used in host table,
          the user should consider move those entries to the LPM table */
        flags = 0;
        host = 0x7fffff03;
        route = 0x7fffff03;
        mask = 0xffffffff;
        /* Units order does not matter */
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            if (!is_device_or_above(unit, JERICHO2)) {
                sand_utils_l3_host_ipv4_s_common_init(unit, 0, &host_ipv4_structure, host, vrf, 0, 0, fec[0]);
                rv = sand_utils_l3_host_ipv4_add(unit, &host_ipv4_structure);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in sand_utils_l3_host_ipv4_add\n");
                    return rv;
                }
            } else {
                sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, route, mask, vrf, flags, 0, fec[0]);
                rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
                    return rv;
                }
            }
        }
    }

    /*** create egress object 2***/
    /* out_sysport unit is already first */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        /* Create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[1], 0, 0, next_hop_mac2, out_vlan);
        rv = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rv;
        }
        encap_id[1] = l3_arp.l3eg_arp_id;

        /* Create FEC and set its properties */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        l3_fec.destination = out_sysport;
        l3_fec.tunnel_gport = ing_intf_out;
        l3_fec.tunnel_gport2 = encap_id[1];
        l3_fec.fec_id = fec[1];
        l3_fec.allocation_flags = lpm_raw_data_enabled ? BCM_L3_WITH_ID : 0;
        rv = sand_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }
        fec[1] = l3_fec.l3eg_fec_id;
        fec_id_used = fec[1];

        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xffff0000;
    flags2 = lpm_raw_data_enabled ? BCM_L3_FLAGS2_RAW_ENTRY : 0;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        flags = (!is_device_or_above(unit, JERICHO2)) ? BCM_L3_RPF : 0;
        printf("fec used is  = 0x%x (%d)\n", fec[1], fec[1]);
        if (lpm_raw_data_enabled) {
            sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, route, mask, vrf, flags, flags2, lpm_payload);
        } else {
            sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, route, mask, vrf, flags, flags2, fec[1]);
        }
        rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
            return rv;
        }
    }

    /* redirect the packet in the pmf acording to the raw payload received in the kaps*/
    if (lpm_raw_data_enabled) {
        if (!BCM_GPORT_IS_TRUNK(lpm_raw_new_dst_port)) {
            BCM_GPORT_LOCAL_SET(dst_gport, lpm_raw_new_dst_port);
        } else {
            dst_gport = lpm_raw_new_dst_port;
        }
        if (is_device_or_above(unit, JERICHO2)){
            rv = dnx_kaps_raw_data_redirect(unit, lpm_payload, 0, 0, dst_gport);
        } else {
            rv = dpp_kaps_raw_data_redirect(unit, lpm_payload, dst_gport);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in function kaps_raw_data_redirect, \n");
            return rv;
        }
    }

    return rv;
}

/*
 *  Runs the regular basic_example on one unit instead of an array of units
 *  Used to call this functionality from Dvapi tests
 */
int basic_example_single_unit(int unit, int in_port, int out_port) {
    int unit_ids[1];
    unit_ids[0] = unit;
    return basic_example_multi_units(unit_ids,1,in_port,out_port);
}

/*
 * Test Scenario
 *     Example of how to deliver IP traffic from one source to multiple receivers.
 *
 * for one device call bcm_l3_main(int unit, int second_unit, int dst_port) with second_unit<0
 * Example: bcm_l3_main(unit,-1, 1);
 *
 * for two devices when one device configures the second device call bcm_l3_main(unit, 1, 1)
 * make sure that master device is unit 0 with modid 0 and slave device is unit 1 with modid 1
 *
 * traffic example for two devices:
 *
 * the cint generates 4 routes and hosts listed below
 *   device 0:
 *   ---------
 *     port: 1           host   6737170e : 103.55.23.14
 *     port: 1           route  78400000 : 120.64.0.0         mask  ffc00000 : 255.192.0.0
 *     port: 2           host   6837170f : 104.55.23.15
 *     port: 2           route  88400000 : 136.64.0.0         mask  ffe00000 : 255.224.0.0
 *
 *   device 1:
 *   ---------
 *     port: 100         host   6537170c : 101.55.23.12
 *     port: 100         route  70000000 : 112.0.0.0          mask  ff000000 : 255.0.0.0
 *     port: 101         host   6637170d : 102.55.23.13
 *     port: 101         route  64000000 : 100.0.0.0          mask  ff800000 : 255.128.0.0
 *
 * Example:
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * bcm_l3_main(0,-1,201);
 * exit;
 *
 *    Sending an ipv4 packet that will be exactly matched (host)
 * tx 1 psrc=200 data=0x00000000553400020500000281000011080045000035000000008000fdcfc0a8000e6537170c000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *     Data: 0x0000cd1d00000000553481000011080045000035000000007f00fecfc0a8000e6537170c000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 *    Sending an ipv4 packet that will be LPM-matched (route)
 * tx 1 psrc=200 data=0x00000000553400020500000281000011080045000035000000008000f1d1c0a8000e88400001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1d00000000553481000011080045000035000000007f00f2d1c0a8000e88400001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 */


int MAX_PORTS=4;

int
bcm_l3_main (
    int unit,
    int second_unit,
    int dst_port)
{
    int CINT_NO_FLAGS = 0;
    int rc;
    int i;
    int egress_port;
    int vlan = 17;
    int tmp_port;
    int vrf = 3;

    int ingress_intf[MAX_PORTS];
    int egress_intf[MAX_PORTS];
    int encap_id[MAX_PORTS];

    int egress_port_list[MAX_PORTS] = {100,101,1,2};
    int egress_port_list_one_device[MAX_PORTS] = {dst_port,dst_port,dst_port,dst_port};

    bcm_port_config_t c;
    bcm_pbmp_t p,u;

    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;
    sand_utils_l3_route_ipv4_s route_ipv4_structure;

    int fec[MAX_PORTS] =  {0};
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34};
    bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};

    if (second_unit<0)
    {

        for (i=0; i<MAX_PORTS; i++) {
            egress_port=egress_port_list_one_device[i];

            bcm_port_config_get(unit, &c);
            BCM_PBMP_ASSIGN(p, c.e);
            rc = bcm_vlan_create(unit, vlan);
            if (rc != BCM_E_NONE && rc!=BCM_E_EXISTS) {
                printf ("bcm_vlan_create failed: %d \n", rc);
                return rc;
            }
            rc = bcm_vlan_port_add(unit, vlan, p, u);
            if (rc != BCM_E_NONE) {
                return rc;
            }

            sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan, BCM_L3_RPF, 0, mac_l3_ingress, vrf);
            eth_rif_structure.urpf_mode = bcmL3IngressUrpfLoose;
            rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
            if (rc != BCM_E_NONE)
            {
                printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            }
            ingress_intf[i] = eth_rif_structure.l3_rif;

            /* Create ARP and set its properties */
            sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id[i], mac_l3_egress, vlan);
            rc = sand_utils_l3_arp_create(unit, &l3_arp);
            if (rc != BCM_E_NONE)
            {
                printf("Error, create egress object ARP only\n");
            }
            encap_id[i] = l3_arp.l3eg_arp_id;

            /* Create FEC and set its properties */
            sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
            BCM_GPORT_LOCAL_SET(l3_fec.destination, egress_port);
            l3_fec.tunnel_gport = ingress_intf[i];
            l3_fec.tunnel_gport2 = encap_id[i];
            l3_fec.fec_id = fec[i];
            rc = sand_utils_l3_fec_create(unit, &l3_fec);
            if (rc != BCM_E_NONE)
            {
                printf("Error, create egress object FEC only\n");
            }
            egress_intf[i] = l3_fec.l3eg_fec_id;

        }
    }
    else
    {
        for (i=0; i<MAX_PORTS; i++) {
            egress_port=egress_port_list[i];

            rc = bcm_vlan_create(unit, vlan);
            if (rc != BCM_E_NONE && rc!=BCM_E_EXISTS) {
                printf ("bcm_vlan_create failed: %d \n", rc);
                return rc;
            }

            bcm_port_config_get(unit, &c);
            BCM_PBMP_ASSIGN(p, c.e);
            rc = bcm_vlan_port_add(unit, vlan, p, u);
            if (rc != BCM_E_NONE) {
                return rc;
            }

            sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan, BCM_L3_RPF, 0, mac_l3_ingress, vrf);
            eth_rif_structure.urpf_mode = bcmL3IngressUrpfLoose;

            sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id[i], mac_l3_egress, vlan);
            sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
            BCM_GPORT_LOCAL_SET(l3_fec.destination, egress_port);
            l3_fec.tunnel_gport = ingress_intf[i];
            l3_fec.tunnel_gport2 = encap_id[i];
            l3_fec.fec_id = fec[i];
            if (is_device_or_above(unit, JERICHO2)) {
                l3_fec.allocation_flags = BCM_L3_WITH_ID;
            }

            if (i<(MAX_PORTS/2)) /*ingress on dev 0 egress on dev 1*/
            {
                rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
                }
                ingress_intf[i] = eth_rif_structure.l3_rif;
                rc = sand_utils_l3_eth_rif_create(second_unit, &eth_rif_structure);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
                }
                ingress_intf[i] = eth_rif_structure.l3_rif;

                rc = sand_utils_l3_arp_create(second_unit, &l3_arp);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, create egress object ARP only\n");
                }
                encap_id[i] = l3_arp.l3eg_arp_id;

                l3_fec.tunnel_gport = ingress_intf[i];
                l3_fec.tunnel_gport2 = encap_id[i];
                rc = sand_utils_l3_fec_create(second_unit, &l3_fec);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, create egress object FEC only\n");
                }
                egress_intf[i] = l3_fec.l3eg_fec_id;

                rc = sand_utils_l3_arp_create(unit, &l3_arp);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, create egress object ARP only\n");
                }
                encap_id[i] = l3_arp.l3eg_arp_id;

                l3_fec.allocation_flags = BCM_L3_WITH_ID;
                rc = sand_utils_l3_fec_create(unit, &l3_fec);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, create egress object FEC only\n");
                }
            }
            else /*egress on dev 0 ingress on dev 1*/
            {
                rc = sand_utils_l3_eth_rif_create(second_unit, &eth_rif_structure);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
                }
                ingress_intf[i] = eth_rif_structure.l3_rif;
                rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
                }
                ingress_intf[i] = eth_rif_structure.l3_rif;

                rc = sand_utils_l3_arp_create(unit, &l3_arp);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, create egress object ARP only\n");
                }
                encap_id[i] = l3_arp.l3eg_arp_id;

                l3_fec.tunnel_gport = ingress_intf[i];
                l3_fec.tunnel_gport2 = encap_id[i];
                rc = sand_utils_l3_fec_create(unit, &l3_fec);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, create egress object FEC only\n");
                }
                egress_intf[i] = l3_fec.l3eg_fec_id;

                rc = sand_utils_l3_arp_create(second_unit, &l3_arp);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, create egress object ARP only\n");
                }
                encap_id[i] = l3_arp.l3eg_arp_id;

                l3_fec.allocation_flags = BCM_L3_WITH_ID;
                rc = sand_utils_l3_fec_create(second_unit, &l3_fec);
                if (rc != BCM_E_NONE)
                {
                    printf("Error, create egress object FEC only\n");
                }
            }
        }
    }


    uint32 host[5];
    uint32 route[5];
    uint32 mask;
    int prefix=1;

    host[0] = ((101%192)<<24)+((55%192)<<16)+((23%192)<<8)+(12%192);
    host[1] = ((102%192)<<24)+((55%192)<<16)+((23%192)<<8)+(13%192);
    host[2] = ((103%192)<<24)+((55%192)<<16)+((23%192)<<8)+(14%192);
    host[3] = ((104%192)<<24)+((55%192)<<16)+((23%192)<<8)+(15%192);
    host[4] = ((105%192)<<24)+((55%192)<<16)+((23%192)<<8)+(16%192);

    route[0] = ((112%192)<<24)+((52%192)<<16)+((22%192)<<8)+(1%192);
    route[1] = ((100%192)<<24)+((53%192)<<16)+((22%192)<<8)+(1%192);
    route[2] = ((120%192)<<24)+((85%192)<<16)+((22%192)<<8)+(1%192);
    route[3] = ((136%192)<<24)+((95%192)<<16)+((22%192)<<8)+(1%192);
    route[4] = ((32%192)<<24)+((60%192)<<16)+((22%192)<<8)+(1%192);


    for (i=0; i<4; i++) {
        if (second_unit < 0)
        {
            tmp_port = egress_port_list_one_device[i%MAX_PORTS];
        }
        else
        {
            tmp_port = egress_port_list[i%MAX_PORTS];
        }

        if (verbose) {
            printf("on egress interface (fec): %d      port: %d           \n", egress_intf[i%MAX_PORTS], tmp_port);
            print_host("host", host[i], vrf);
        }
        add_host_ipv4 (unit, host[i], vrf, egress_intf[i%MAX_PORTS], 0, 0);
        if (second_unit >= 0)
        {
            rc = add_host_ipv4 (second_unit, host[i], vrf, egress_intf[i%MAX_PORTS], 0, 0);
            if (rc != BCM_E_NONE) {
              printf("Error. add_host failed: $d\n", rc);
              return BCM_E_FAIL;
            }
        }
    }

    for (i=0; i<4; i++) {
        if (second_unit < 0)
        {
            tmp_port = egress_port_list_one_device[i%MAX_PORTS];
        }
        else
        {
            tmp_port = egress_port_list[i%MAX_PORTS];
        }
        prefix--;
        if (prefix == 0) {
            prefix = 24;
        }
        mask = 0xffffffff << prefix;
        route[i] = route[i] & mask;
        sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, route[i], 0xfffff000, vrf, 0, 0, egress_intf[i%MAX_PORTS]);
        rc = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
            return rc;
        }

        if (second_unit >= 0)
        {
            sand_utils_l3_route_ipv4_s_common_init(second_unit, 0, &route_ipv4_structure, route[i], 0xfffff000, vrf, 0, 0, egress_intf[i%MAX_PORTS]);
            rc = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
            if (rc != BCM_E_NONE)
            {
                printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
                return rc;
            }
        }
        if (verbose) {
            printf("on egress interface (fec): %d      port: %d     \n", egress_intf[i%MAX_PORTS], tmp_port);
            print_route("route", route[i], mask, vrf);
        }
    }

    return 0;
}

/*
 * Test Scenario
 *     Example of Ipv6 unicast.
 *
 *  The purpose of this cint is to create four interfaces (one as an IN-RIF and 3 as OUT-RIFs) and 6 hosts/routes with the same IPV6 DIP and VRF.
 *   1.  Host -  host with interface 1 as a FEC.
 *   2.  Route - route (64 bit mask) with interface 2 as a FEC.
 *   3.  Default - Default route (128 bit mask) with interface 3 as FEC.
 *
 * Example:
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * bcm_l3_ip6_uc_priority(0,200,201,202,203,0);
 * exit;
 *
 *    Send IPV6 paket - run number 1
 * tx 1 psrc=200 data=0x00000000551000001964f7f2810007d086dd60000000004906800000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00006630000000005530810067da86dd606000000049067f0000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 *    Send IPV6 paket - run number 2
 * tx 1 psrc=200 data=0x00000000551000001964f7f2810007d086dd60000000004906800000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00006650000000005550810067e486dd606000000049067f0000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 *    Send IPV6 paket - run number 3
 * tx 1 psrc=200 data=0x00000000551000001964f7f2810007d086dd60000000004906800000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00006670000000005570810067ee86dd606000000049067f0000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 */
int NOF_INTERFACES = 4;

/*
 * Note: tcam=1 is not supported by JR2.
 */
int
bcm_l3_ip6_uc_priority (
    int unit,
    int port0,
    int port1,
    int port2,
    int port3,
    int tcam)
{
    int rc,i;
    bcm_l3_intf_t l3if;
    bcm_if_t l3egid;
    bcm_mac_t mac_l3_ingress_add = {0x00, 0x00, 0x00, 0x00, 0x55, 0x00};
    bcm_mac_t mac_l3_engress_add = {0x00, 0x00, 0x00, 0x00, 0x66, 0x00};

    int vlan[NOF_INTERFACES];/*Each interface VLAN and VRF */
    uint8 mac_last_byte[NOF_INTERFACES];/*All interfaces has the same my MAC/dest MAC differ only in the last byte */
    int ingress_intf[NOF_INTERFACES] = {0};
    int encap_id[NOF_INTERFACES] = {0};
    int egress_intf[NOF_INTERFACES] = {0};
    int ports[NOF_INTERFACES] = {0};

    sand_utils_l3_port_s l3_port_structure;
    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;

    /*Set each interface parameters*/
    vlan[0] = 2000;
    mac_last_byte[0] = 0x10;
    for (i=1; i < NOF_INTERFACES; i++) {
        vlan[i] = vlan[i-1]+10;
        mac_last_byte[i] = mac_last_byte[i-1]+0x20;
    }
    if (is_device_or_above(unit, JERICHO2)) {
        sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, port1, vlan[0]);
        rc = sand_utils_l3_port_set(unit, &l3_port_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_port_set\n");
            return rc;
        }
        sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, port0, vlan[0]);
        rc = sand_utils_l3_port_set(unit, &l3_port_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_port_set\n");
            return rc;
        }
        sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, port2, vlan[0]);
        rc = sand_utils_l3_port_set(unit, &l3_port_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_port_set\n");
            return rc;
        }
    }
    ports[0] = port0;
    ports[1] = port1;
    ports[2] = port2;
    ports[3] = port3;

    for (i=0; i < NOF_INTERFACES; i++) {

        /*Create L3 interface*/
        bcm_l3_intf_t_init(l3if);
        if(rc == BCM_E_EXISTS) {
          printf ("vlan %d aleady exist \n", vlan[i]);
        }

        mac_l3_ingress_add[5] = mac_last_byte[i];

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan[i], 0, 0, mac_l3_ingress_add, vlan[i]);
        rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create\n");
        }
        ingress_intf[i] = eth_rif_structure.l3_rif;

        /*Create L3 egress*/
        mac_l3_engress_add[5] = mac_last_byte[i];
        /* create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id[i], mac_l3_engress_add, vlan[i]);
        rc = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rc;
        }
        encap_id[i] = l3_arp.l3eg_arp_id;

        /* create FEC and set its properties */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        BCM_GPORT_LOCAL_SET(l3_fec.destination, ports[i]);
        l3_fec.tunnel_gport = ingress_intf[i];
        l3_fec.tunnel_gport2 = encap_id[i];
        l3_fec.fec_id = egress_intf[i];
        l3_fec.allocation_flags = 0;
        rc= sand_utils_l3_fec_create(unit, &l3_fec);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rc;
        }
        egress_intf[i] = l3_fec.l3eg_fec_id;

    }

    bcm_ip6_t route = {0x1,0,0x16,0,0x35,0,0x70,0,0,0,0xdb,0x7,0,0,0,0};
    bcm_ip6_t mask   = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0,0,0,0,0,0,0,0};
    bcm_ip6_t fullMask   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bcm_l3_host_t l3host;
    bcm_l3_route_t l3rt;



    /*Create Host*/
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags =  BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr,route,16);
    l3host.l3a_vrf = vlan[0];
    /* dest is FEC + OutLif */
    l3host.l3a_intf = egress_intf[1]; /* fec */
    if (tcam == 1) {
        l3host.encap_id = encap_id[1];   /* outlif */
    }
    rc = bcm_l3_host_add(unit, &l3host);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_host_add failed: %d \n", rc);
      return rc;
    }

    /*Create route*/
    bcm_l3_route_t_init(&l3rt);
    l3rt.l3a_flags = BCM_L3_IP6;
    sal_memcpy(&(l3rt.l3a_ip6_net),(route),16);
    sal_memcpy(&(l3rt.l3a_ip6_mask),(mask),16);
    l3rt.l3a_vrf = vlan[0];
    l3rt.l3a_intf = egress_intf[2]; /* fec */;
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_route_add failed: %d \n", rc);
      return rc;
    }

    /*Default route*/
    bcm_l3_route_t_init(&l3rt);
    l3rt.l3a_flags = BCM_L3_IP6;
    sal_memcpy(&(l3rt.l3a_ip6_net),(route),16);
    sal_memcpy(&(l3rt.l3a_ip6_mask),(fullMask),16);
    l3rt.l3a_vrf = vlan[0];
    l3rt.l3a_intf = egress_intf[3]; /* fec */;
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_route_add failed: %d \n", rc);
      return rc;
    }

    return rc;
}

/*
 * Test Scenario
 *   test ipv4 host with different destination types
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * l3_host_destination_example(unit,50,200,201);
 * exit;
 *
 * Packet to hit entry with FEC destination:
 * tx 1 psrc=200 data=0x000c00020000000007000100810000cc080045000035000000008000b743c0800101c0020202000102030405060708090a0b0c0d0e0f10
 *
 * Packet to hit entry with Port + outLIF
 * tx 1 psrc=200 data=0x000c00020000000007000100810000cc080045000035000000008000b742c0800101c0020203000102030405060708090a0b0c0d0e0f10
 *
 * Packet to hit entry with FEC + ARP + RIF
 * tx 1 psrc=200 data=0x000c00020000000007000100810000cc080045000035000000008000b741c0800101c0020204000102030405060708090a0b0c0d0e0f10
 */
int
l3_host_destination_example(
    int unit,
    int vid,
    int in_port,
    int out_port)
{
    int rv = BCM_E_NONE;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int fec = 42769;
    int vrf = 1;
    int encap_id = 900;         /* ARP-Link-layer */
    int host_encap_id = 0;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 host = 0xC0020202;

    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);

    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;
    char *err_proc_name;
    char *proc_name;

    proc_name = "l3_host_destination_example";
    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * encap id for jr2 must be > 2k
         */
        encap_id = 0x1384;
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set intf_out\n", proc_name);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in\n", proc_name);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create(unit, fec, intf_out, encap_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }

    /*
     * 6. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &encap_id, arp_next_hop_mac, vid);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * 8. Add host entries
     */
    rv = add_host_ipv4(unit, host, vrf, fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    rv = add_host_ipv4(unit, host+1, vrf, encap_id, 0, gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    BCM_GPORT_FORWARD_PORT_SET(gport, fec);
    rv = add_host_ipv4(unit, host+2, vrf, intf_in, encap_id, gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    return rv;
}


/*
 * Test Scenario
 *     Example of Ipv6 unicast and multicast.
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_qos_l3_remark.c
 * cint
 * qos_map_l3_remark(0,0x7,0x2);
 * exit;
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_examples.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * bcm_l3_ip6(0,-1,200,201,-1);
 * exit;
 *
 *    IPV6 UC private packet
 * tx 1 psrc=200 data=0x0000000055340000000000018100001186dd60000000004906800000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00000000cd1d0000000055348100001286dd600000000049067f0000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    IPV6 UC public packet
 * tx 1 psrc=200 data=0x0000000055340000000000018100001386dd60000000004906800000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00000000cd1d0000000055348100001486dd600000000049067f0000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    IPV6 MC packet
 * tx 1 psrc=200 data=0x3333030201000000000000018100001186dd60000000004906ff01001600350070000000db0700000000ff1e0d0c0b0a09080706050403020100000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x3333030201000000000055348100001286dd60000000004906fe01001600350070000000db0700000000ff1e0d0c0b0a09080706050403020100000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *    Source port: 0, Destination port: 0
 *    Data: 0x3333030201000000000055348100001286dd60000000004906fe01001600350070000000db0700000000ff1e0d0c0b0a09080706050403020100000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    IPV6 MC2 packet
 * tx 1 psrc=200 data=0x3333030201120000000000018100001186dd60000000004906ff01001600350070000000db0700000000ff1e0d0c0b0a09080706050403020112000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x3333030201120000000055348100001286dd60000000004906fe01001600350070000000db0700000000ff1e0d0c0b0a09080706050403020112000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *    Source port: 0, Destination port: 0
 *    Data: 0x3333030201120000000055348100001286dd60000000004906fe01001600350070000000db0700000000ff1e0d0c0b0a09080706050403020112000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */


/* enable to set / unset UC routes
   for MC & UC routes use:
         bcm_l3_ip6(int unit, int second_unit ,int in_port, int out_port)
*/
int add_ipv6_uc_routes=1;

int
bcm_l3_ip6 (
    int unit,
    int second_unit,
    int in_port,
    int out_port,
    int preserv_dsp_port)
{
    int rv;
    bcm_gport_t dst_gport;

    /* redirect the packet in the pmf acording to the raw payload received in the kaps*/
    if (lpm_raw_data_enabled) {
        BCM_GPORT_LOCAL_SET(dst_gport, lpm_raw_new_dst_port);
        if (is_device_or_above(unit, JERICHO2)){
            rv = dnx_kaps_raw_data_redirect(unit, lpm_payload, 0, 0, dst_gport);
        } else {
            rv = dpp_kaps_raw_data_redirect(unit, lpm_payload, dst_gport);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in function kaps_raw_data_redirect, \n");
            return rv;
        }
    }

    return bcm_l3_ip6_with_ttl(unit, second_unit, in_port, out_port, 0);
}

/* 
 * fall2bridge ive setting
 */
int
l3_ip6_fall2bridge_ive (
    int unit,
    int in_port,
    int in_vlan)
{
    int rv = 0;
    bcm_vlan_port_t vlan_port;
    bcm_port_tpid_class_t port_tpid_class;
    int outer_vid = 100;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.match_vlan = in_vlan;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : in_vlan;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, in_vlan, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    bcm_port_tpid_class_t_init(&port_tpid_class);

    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.port = in_port;
    if (is_device_or_above(unit, JERICHO2)){
        /* Create Tag-Struct for S-TAG */
        port_tpid_class.tag_format_class_id = 4;
    } else {
        port_tpid_class.tag_format_class_id = 2;
    }

    printf("bcm_port_tpid_class_set(port=%d, tag_format_class_id=%d)\n", port_tpid_class.port,
           port_tpid_class.tag_format_class_id);

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set( in_port )\n");
        return rv;
    }

    rv = vlan_translate_ive_eve_translation_set(unit, vlan_port.vlan_port_id,     /* lif */
                                                0x8100,     /* outer_tpid */
                                                0x8100,     /* inner_tpid */
                                                bcmVlanActionAdd,       /* outer_action */
                                                bcmVlanActionNone,  /* inner_action */
                                                outer_vid,    /* new_outer_vid */
                                                0,  /* new_inner_vid */
                                                7, /* vlan_edit_profile */
                                                port_tpid_class.tag_format_class_id,        /* tag_format */
                                                TRUE        /* is_ive */
        );
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_ive_eve_translation_set\n");
        return rv;
    }

    return rv;
}

int
bcm_l3_ip6_with_ttl (
    int unit,
    int second_unit,
    int in_port,
    int out_port,
    int ttl_threshold)
{
    MAX_PORTS=2;

    int CINT_NO_FLAGS = 0;
    int vrf = 3;
    int vrf_public = 0;
    int rc;
    int in_vlan = 17;
    int out_vlan = 18;
    int nof_mc_members = 2;
    int ipmc_index[MAX_PORTS] = {6010, 6011};

    int ingress_intf[MAX_PORTS*2];
    int egress_intf[MAX_PORTS];
    int encap_id[MAX_PORTS];
    bcm_multicast_t mc_id[MAX_PORTS];

    /* used for multi device: */
    int second_ingress_intf[MAX_PORTS];
    int ingress_port_list[MAX_PORTS];
    int egress_port_list[MAX_PORTS];

    bcm_if_t cud;
    int trap_id, port_ndx;
    bcm_rx_trap_config_t trap_config;

    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34}; /* incoming DA (myMAc) */
    bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */

    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;
    sand_utils_l3_route_ipv6_s route_ipv6_structure;

    uint32 fwd_only_flags = 0;

    bcm_ip6_t no_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t full_mask = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if (lpm_raw_data_enabled) {
        vrf_public = vrf = 1;
    }

    if (fall2bridge_ive_enable) {
        nof_mc_members = 0;
    }

    if (!is_device_or_above(unit, JERICHO2))
    {
        /*
         * For JR1, only one global MSBs My-MAC was supported. bcm_l2_station_add API is used to assign a global MSBs my-MAC to a VSI.
         * For JR2, there is no need to call bcm_l2_station_add. VSI global MSBs my-MAC assignment is managed internally using the API bcm_l3_intf_create.
         */
        int units_ids[2] = {unit, second_unit};
        int nof_units = (second_unit == -1) ? 1 : 2;
        int i = 0;
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            int station_id;
            bcm_l2_station_t station;
            bcm_l2_station_t_init(&station);
            /* set my-Mac global MSB */
            station.flags = 0;
            sal_memcpy(station.dst_mac, mac_l3_ingress, 6);
            station.src_port_mask = 0; /* port is not valid */
            station.vlan_mask = 0; /* vsi is not valid */
            station.dst_mac_mask[0] = station.dst_mac_mask[1] = station.dst_mac_mask[2] =
            station.dst_mac_mask[3] = station.dst_mac_mask[4] = station.dst_mac_mask[5] = 0xff; /* dst_mac my-Mac MSB mask is -1 */

            rc = bcm_l2_station_add(unit, &station_id, &station);
            if (rc != BCM_E_NONE) {
                printf("Error, in bcm_l2_station_add\n");
                return rc;
            }
        }
    }

    if (second_unit<0)
    {
        /* create in-rif */
        rc = bcm_vlan_create(unit, in_vlan);
        if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
            printf("failed to create vlan %d", in_vlan);
        }

        rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
        if (rc != BCM_E_NONE) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
            return rc;
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, in_vlan, 0, 0, mac_l3_ingress, vrf);
        eth_rif_structure.ing_qos_map_id = qos_map_id_l3_ingress_get(unit);
        if (ttl_threshold > 0)
        {
            printf("TTL Threshold configuration\n");
            eth_rif_structure.ttl = ttl_threshold;
            eth_rif_structure.ttl_valid = 1;
        }
        rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create\n");
        }
        ingress_intf[0] = eth_rif_structure.l3_rif;

        /* create out-rif */
        rc = bcm_vlan_create(unit, out_vlan);
        if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
            printf("failed to create vlan %d", out_vlan);
        }

        rc = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
        if (rc != BCM_E_NONE) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
            return rc;
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, out_vlan, 0, 0, mac_l3_ingress, vrf);
        eth_rif_structure.ing_qos_map_id = qos_map_id_l3_ingress_get(unit);
        if (ttl_threshold > 0)
        {
            printf("TTL Threshold configuration\n");
            eth_rif_structure.ttl = ttl_threshold;
            eth_rif_structure.ttl_valid = 1;
        }
        rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create\n");
        }
        ingress_intf[1] = eth_rif_structure.l3_rif;

        /* create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id[0], mac_l3_egress, out_vlan);
        rc = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rc;
        }
        encap_id[0] = l3_arp.l3eg_arp_id;

        /* create FEC and set its properties */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        BCM_GPORT_LOCAL_SET(l3_fec.destination, out_port);
        l3_fec.tunnel_gport = ingress_intf[1];
        l3_fec.tunnel_gport2 = encap_id[0];
        l3_fec.fec_id = egress_intf[0];
        l3_fec.allocation_flags = 0;
        rc= sand_utils_l3_fec_create(unit, &l3_fec);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rc;
        }
        egress_intf[0] = l3_fec.l3eg_fec_id;

        /*Add a public IPV6 entry for jericho private-public tests*/
        /*the out_vlan is incremented by 2 for public-forwarding*/
        if (is_device_or_above(unit,JERICHO)) {
            /* create in-rif */
            /* TTL/hop-limit threshold in valid only only in the egress, so for in-rif it is set to max - 255 */
            rc = bcm_vlan_create(unit, in_vlan + 2);
            if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
                printf("failed to create vlan %d", in_vlan + 2);
            }

            rc = bcm_vlan_gport_add(unit, in_vlan + 2, in_port, 0);
            if (rc != BCM_E_NONE) {
                printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan + 2);
                return rc;
            }

            vrf_public = (lpm_raw_data_enabled) ? 1 : 0;
            sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, (in_vlan+2), 0, BCM_L3_INGRESS_GLOBAL_ROUTE, mac_l3_ingress, vrf_public);
            eth_rif_structure.ing_qos_map_id = qos_map_id_l3_ingress_get(unit);
            rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
            if (rc != BCM_E_NONE)
            {
                printf("Error, sand_utils_l3_eth_rif_create\n");
            }
            ingress_intf[2] = eth_rif_structure.l3_rif;

            /* create out-rif */
            rc = bcm_vlan_create(unit, out_vlan + 2);
            if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
                printf("failed to create vlan %d", out_vlan + 2);
            }

            rc = bcm_vlan_gport_add(unit, out_vlan + 2, out_port, 0);
            if (rc != BCM_E_NONE) {
                printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan + 2);
                return rc;
            }

            sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, (out_vlan+2), 0, 0, mac_l3_ingress, 0/*vrf*/);
            eth_rif_structure.ing_qos_map_id = qos_map_id_l3_ingress_get(unit);
            rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
            if (rc != BCM_E_NONE)
            {
                printf("Error, sand_utils_l3_eth_rif_create\n");
            }
            ingress_intf[3] = eth_rif_structure.l3_rif;

            /* create ARP and set its properties */
            sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id[1], mac_l3_egress, (out_vlan+2));
            rc = sand_utils_l3_arp_create(unit, &l3_arp);
            if (rc != BCM_E_NONE)
            {
                printf("Error, create egress object ARP only\n");
                return rc;
            }
            encap_id[1] = l3_arp.l3eg_arp_id;

            /* create FEC and set its properties */
            sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
            BCM_GPORT_LOCAL_SET(l3_fec.destination, out_port);
            l3_fec.tunnel_gport = ingress_intf[1];
            l3_fec.tunnel_gport2 = encap_id[1];
            l3_fec.fec_id = egress_intf[1];
            l3_fec.allocation_flags = 0;
            rc= sand_utils_l3_fec_create(unit, &l3_fec);
            if (rc != BCM_E_NONE)
            {
                printf("Error, create egress object FEC only\n");
                return rc;
            }
            egress_intf[1] = l3_fec.l3eg_fec_id;
        }

        /* create MC group for MC IPV6 entry:
         * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
        mc_id[0] = create_ip_mc_group(unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[0], out_port, nof_mc_members, out_vlan, FALSE);
        printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[0], ipmc_index[0], nof_mc_members, out_vlan);

        /* Add vlan member to each outgoing port */
        for(port_ndx = 0; port_ndx < nof_mc_members; ++port_ndx) {
            rc = bcm_vlan_gport_add(unit, out_vlan, out_port+port_ndx, 0);
            if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", out_port+port_ndx, out_vlan);
                return rc;
            }
        }

        if (is_device_or_above(unit, JERICHO2)) {
            bcm_multicast_replication_t replications;
            replications.encap1 = in_vlan;
            replications.port = in_port;
            rc = bcm_multicast_add(unit, mc_id[0], BCM_MULTICAST_INGRESS_GROUP, 1, &replications);
            if (rc != BCM_E_NONE)
            {
                printf("Error, bcm_multicast_add\n");
                return rc;
            }
        }
        else {
            /* add also the in_port (with the in_vlan) to the MC group */
            rc = bcm_multicast_l3_encap_get(unit, mc_id[0], in_port, in_vlan, &cud);
            if (rc != BCM_E_NONE) {
                printf("Error, in bcm_petra_multicast_l3_encap_get, mc_group mc_group \n");
                return rc;
            }
            rc = bcm_multicast_ingress_add(unit, mc_id[0], in_port, cud);
            if (rc != BCM_E_NONE) {
                printf("Error, in bcm_multicast_ingress_add, ipmc_index %d dest_gport $in_port \n", mc_id[0]);
                return rc;
            }
        }
    }
    else /* second_unit>=0 */
    {
        /* ingress on dev 0 egress on dev 1 */
        BCM_GPORT_MODPORT_SET(ingress_port_list[0], unit, in_port);
        BCM_GPORT_MODPORT_SET(egress_port_list[0], second_unit, out_port);

        /* create in-rif */
        rc = bcm_vlan_create(unit, in_vlan);
        if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
            printf("failed to create vlan %d", in_vlan);
        }

        rc = bcm_vlan_gport_add(unit, in_vlan, ingress_port_list[0], 0);
        if (rc != BCM_E_NONE) {
            printf("fail add port(0x%08x) to vlan(%d)\n", ingress_port_list[0], in_vlan);
            return rc;
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, in_vlan, 0, 0, mac_l3_ingress, vrf);
        eth_rif_structure.ing_qos_map_id = qos_map_id_l3_ingress_get(unit);
        rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create\n");
        }
        ingress_intf[0] = eth_rif_structure.l3_rif;

        /* create out-rif */
        rc = bcm_vlan_create(second_unit, out_vlan);
        if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
            printf("failed to create vlan %d", out_vlan);
        }

        rc = bcm_vlan_gport_add(second_unit, out_vlan, egress_port_list[0], 0);
        if (rc != BCM_E_NONE) {
            printf("fail add port(0x%08x) to vlan(%d)\n", egress_port_list[0], out_vlan);
            return rc;
        }

        sand_utils_l3_eth_rif_s_common_init(second_unit, 0, &eth_rif_structure, out_vlan, 0, 0, mac_l3_ingress, vrf);
        eth_rif_structure.ing_qos_map_id = qos_map_id_l3_ingress_get(unit);
        rc = sand_utils_l3_eth_rif_create(second_unit, &eth_rif_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create\n");
        }
        second_ingress_intf[0] = eth_rif_structure.l3_rif;

        /* create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id[0], mac_l3_egress, out_vlan);
        rc = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rc;
        }
        encap_id[0] = l3_arp.l3eg_arp_id;

        /* create FEC and set its properties */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        BCM_GPORT_LOCAL_SET(l3_fec.destination, egress_port_list[0]);
        l3_fec.tunnel_gport = second_ingress_intf[0];
        l3_fec.tunnel_gport2 = encap_id[0];
        l3_fec.fec_id = egress_intf[0];
        l3_fec.allocation_flags = 0;
        rc= sand_utils_l3_fec_create(unit, &l3_fec);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rc;
        }
        egress_intf[0] = l3_fec.l3eg_fec_id;

        /* create MC group for MC IPV6 entry:
         * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
        mc_id[0] = create_ip_mc_group(second_unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[0], egress_port_list[0], nof_mc_members, out_vlan, FALSE);
        printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[0], ipmc_index[0], nof_mc_members, out_vlan);

        /* egress on dev 0 ingress on dev 1 */
        BCM_GPORT_MODPORT_SET(egress_port_list[1], unit, out_port);
        BCM_GPORT_MODPORT_SET(ingress_port_list[1], second_unit, in_port);

        /* create in-rif */
        rc = bcm_vlan_create(second_unit, in_vlan);
        if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
            printf("failed to create vlan %d", in_vlan);
        }

        rc = bcm_vlan_gport_add(second_unit, in_vlan, ingress_port_list[1], 0);
        if (rc != BCM_E_NONE) {
            printf("fail add port(0x%08x) to vlan(%d)\n", ingress_port_list[1], in_vlan);
            return rc;
        }

        sand_utils_l3_eth_rif_s_common_init(second_unit, 0, &eth_rif_structure, in_vlan, 0, 0, mac_l3_ingress, vrf);
        eth_rif_structure.ing_qos_map_id = qos_map_id_l3_ingress_get(unit);
        rc = sand_utils_l3_eth_rif_create(second_unit, &eth_rif_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create\n");
        }
        ingress_intf[1] = eth_rif_structure.l3_rif;

        /* create out-rif */
        rc = bcm_vlan_create(unit, out_vlan);
        if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
            printf("failed to create vlan %d", out_vlan);
        }

        rc = bcm_vlan_gport_add(unit, out_vlan, egress_port_list[1], 0);
        if (rc != BCM_E_NONE) {
            printf("fail add port(0x%08x) to vlan(%d)\n", egress_port_list[1], out_vlan);
            return rc;
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, out_vlan, 0, 0, mac_l3_ingress, vrf);
        eth_rif_structure.ing_qos_map_id = qos_map_id_l3_ingress_get(unit);
        rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create\n");
        }
        second_ingress_intf[1] = eth_rif_structure.l3_rif;

        /* create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id[0], mac_l3_egress, out_vlan);
        rc = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rc;
        }
        encap_id[1] = l3_arp.l3eg_arp_id;

        /* create FEC and set its properties */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        BCM_GPORT_LOCAL_SET(l3_fec.destination, egress_port_list[1]);
        l3_fec.tunnel_gport = second_ingress_intf[1];
        l3_fec.tunnel_gport2 = encap_id[1];
        l3_fec.fec_id = egress_intf[1];
        l3_fec.allocation_flags = 0;
        rc= sand_utils_l3_fec_create(unit, &l3_fec);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rc;
        }
        egress_intf[1] = l3_fec.l3eg_fec_id;

        /* create MC group for MC IPV6 entry:
         * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
        mc_id[1] = create_ip_mc_group(unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[1], egress_port_list[1], nof_mc_members, out_vlan, FALSE);
        printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[1], ipmc_index[1], nof_mc_members, out_vlan);
    }

    bcm_ip6_t route0;
    bcm_ip6_t route1;
    bcm_ip6_t route2;
    bcm_ip6_t route3;
    bcm_ip6_t route4;
    bcm_ip6_t route5;
    bcm_ip6_t mask;

    /* UC IPV6 DIP: */
    route0[15]= 0; /* LSB */
    route0[14]= 0;
    route0[13]= 0;
    route0[12]= 0;
    route0[11]= 0x7;
    route0[10]= 0xdb;
    route0[9] = 0;
    route0[8] = 0;
    route0[7] = 0;
    route0[6] = 0x70;
    route0[5] = 0;
    route0[4] = 0x35;
    route0[3] = 0;
    route0[2] = 0x16;
    route0[1] = 0;
    route0[0] = 0x1; /* MSB */

    route1[15]= 0;
    route1[14]= 0;
    route1[13]= 0;
    route1[12]= 0;
    route1[11]= 0x7;
    route1[10]= 0xdb;
    route1[9] = 0;
    route1[8] = 0;
    route1[7] = 0;
    route1[6] = 0x64;
    route1[5] = 0;
    route1[4] = 0x35;
    route1[3] = 0;
    route1[2] = 0x16;
    route1[1] = 0;
    route1[0] = 0x1;

    route2[15]= 0;
    route2[14]= 0;
    route2[13]= 0;
    route2[12]= 0;
    route2[11]= 0x7;
    route2[10]= 0xdb;
    route2[9] = 0;
    route2[8] = 0;
    route2[7] = 0;
    route2[6] = 0x78;
    route2[5] = 0;
    route2[4] = 0x55;
    route2[3] = 0;
    route2[2] = 0x16;
    route2[1] = 0;
    route2[0] = 0x1;

    route3[15]= 0;
    route3[14]= 0;
    route3[13]= 0;
    route3[12]= 0;
    route3[11]= 0x7;
    route3[10]= 0xdb;
    route3[9] = 0;
    route3[8] = 0;
    route3[7] = 0;
    route3[6] = 0x88;
    route3[5] = 0;
    route3[4] = 0x78;
    route3[3] = 0;
    route3[2] = 0x16;
    route3[1] = 0;
    route3[0] = 0x1;

    /* UC IPV6 DIP MASK: */
    mask[15]= 0;
    mask[14]= 0;
    mask[13]= 0;
    mask[12]= 0;
    mask[11]= 0;
    mask[10]= 0;
    mask[9] = 0;
    mask[8] = 0;
    mask[7] = 0xff;
    mask[6] = 0xff;
    mask[5] = 0xff;
    mask[4] = 0xff;
    mask[3] = 0xff;
    mask[2] = 0xff;
    mask[1] = 0xff;
    mask[0] = 0xff;

    /* MC IPV6 DIP */
    route4[15]= 0x0;
    route4[14]= 0x1;
    route4[13]= 0x2;
    route4[12]= 0x3;
    route4[11]= 0x4;
    route4[10]= 0x5;
    route4[9] = 0x6;
    route4[8] = 0x7;
    route4[7] = 0x8;
    route4[6] = 0x9;
    route4[5] = 0xa;
    route4[4] = 0xb;
    route4[3] = 0xc;
    route4[2] = 0xd;
    route4[1] = 0x1e;
    route4[0] = 0xff;

    /* MC IPV6 DIP 2 */
    route5[15]= 0x12;
    route5[14]= 0x1;
    route5[13]= 0x2;
    route5[12]= 0x3;
    route5[11]= 0x4;
    route5[10]= 0x5;
    route5[9] = 0x6;
    route5[8] = 0x7;
    route5[7] = 0x8;
    route5[6] = 0x9;
    route5[5] = 0xa;
    route5[4] = 0xb;
    route5[3] = 0xc;
    route5[2] = 0xd;
    route5[1] = 0x1e;
    route5[0] = 0xff;

    int i;
    for (i=0; i<6; i++)
    {
        if (i == 4)
        {
            /* add MC entry */
            rc = find_ipv6_ipmc(unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0]);
            if (rc != BCM_E_NONE)
            {
                if (lpm_raw_data_enabled) {
                    rc = add_ipv6_ipmc_raw(unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0], lpm_payload, vrf);
                } else {
                    rc = add_ipv6_ipmc(unit, route4, no_mask, route0, no_mask, in_vlan, vrf, 0, 0, mc_id[0], 0, 0);
                }
                if (rc != BCM_E_NONE) {
                    printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                    return BCM_E_FAIL;
                }
            }
            if (second_unit>=0)
            {
                rc = find_ipv6_ipmc(second_unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1]);
                if (rc != BCM_E_NONE) {
                    if (lpm_raw_data_enabled) {
                        rc = add_ipv6_ipmc_raw(second_unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1], lpm_payload, vrf);
                    } else {
                        rc = add_ipv6_ipmc(second_unit, route4, no_mask, route0, no_mask, in_vlan, vrf, 0, 0, mc_id[1], 0, 0);
                    }
                    if (rc != BCM_E_NONE) {
                        printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                        return BCM_E_FAIL;
                    }
                }
            }
        }
        else if (i == 5)
        {
            /* add MC entry */
            rc = find_ipv6_ipmc(unit, route5 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0]);
            if (rc != BCM_E_NONE)
            {
                if (lpm_raw_data_enabled) {
                    rc = add_ipv6_ipmc_raw(unit, route5 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0], lpm_payload, vrf);
                } else {
                    rc = add_ipv6_ipmc(unit, route5, no_mask, route0, no_mask, in_vlan, vrf, 0, 0, mc_id[0], 0, 0);
                }
                if (rc != BCM_E_NONE) {
                    printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                    return BCM_E_FAIL;
                }
            }
            if (second_unit>=0)
            {
                rc = find_ipv6_ipmc(second_unit, route5 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1]);
                if (rc != BCM_E_NONE) {
                    if (lpm_raw_data_enabled) {
                        rc = add_ipv6_ipmc_raw(second_unit, route5 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1], lpm_payload, vrf);
                    } else {
                        rc = add_ipv6_ipmc(second_unit, route5, no_mask, route0, no_mask, in_vlan, vrf, 0, 0, mc_id[1], 0, 0);
                    }
                    if (rc != BCM_E_NONE) {
                        printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                        return BCM_E_FAIL;
                    }
                }
            }
        }
        else
        {
            if (add_ipv6_uc_routes==1) {
                /* change the prefix length, for IPV6 UC */
                if (i == 1) {
                    mask[7] = 0xfc;
                }
                else if (i == 2) {
                    mask[7] = 0xf0;
                }
                else if (i == 3) {
                    mask[7] = 0xc0;
                }
                if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
                {
                    fwd_only_flags = BCM_L3_FLAGS2_FWD_ONLY;
                }

                /* add UC entries */
                if (second_unit<0) {
                    if (i == 0) {
                        if (lpm_raw_data_enabled) {
                            rc = add_route_ipv6_raw(unit, route0, mask, vrf, lpm_payload);
                        } else {
                            sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, route0, mask, vrf, 0, fwd_only_flags, egress_intf[0]);
                            rc = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                        }
                        /*Add a public IPV6 entry for jericho private-public tests*/
                        /*the out_vlan is incremented by 1 for public-forwarding*/
                        if (is_device_or_above(unit,JERICHO)) {
                            if (lpm_raw_data_enabled) {
                                rc = add_route_ipv6_raw(unit, route0, mask, vrf_public, lpm_payload);
                            } else {
                                sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, route0, mask, vrf_public, 0, fwd_only_flags, egress_intf[1]);
                                rc = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                            }
                        }
                    }
                    else if (i == 1) {
                        if (lpm_raw_data_enabled) {
                            rc = add_route_ipv6_raw(unit, route1, mask, vrf, lpm_payload);
                        } else {
                            sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, route1, mask, vrf, 0, fwd_only_flags, egress_intf[0]);
                            rc = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                        }
                    }
                    else if (i == 2) {
                        if (lpm_raw_data_enabled) {
                            rc = add_route_ipv6_raw(unit, route2, mask, vrf, lpm_payload);
                        } else {
                            sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, route2, mask, vrf, 0, fwd_only_flags, egress_intf[0]);
                            rc = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                        }
                    }
                    else { /*i == 3*/
                        if (lpm_raw_data_enabled) {
                            rc = add_route_ipv6_raw(unit, route3, mask, vrf, lpm_payload);
                        } else {
                            sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, route3, mask, vrf, 0, fwd_only_flags, egress_intf[0]);
                            rc = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                        }
                    }
                }
                else /* second_unit>=0 */
                {
                    if (i == 0) {
                        if (lpm_raw_data_enabled) {
                            rc = add_route_ipv6_raw(unit, route0, mask, vrf, lpm_payload);
                        } else {
                            sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, route0, mask, vrf, 0, fwd_only_flags, egress_intf[i%MAX_PORTS]);
                            rc = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                        }
                    } else if (i == 1) {
                        if (!lpm_raw_data_enabled) {
                            rc = add_route_ipv6_raw(unit, route1, mask, vrf, lpm_payload);
                        } else {
                            sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, route1, mask, vrf, 0, fwd_only_flags, egress_intf[i%MAX_PORTS]);
                            rc = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                        }
                    } else if (i == 2) {
                        if (lpm_raw_data_enabled) {
                            rc = add_route_ipv6_raw(second_unit, route2, mask, vrf, lpm_payload);
                        } else {
                            sand_utils_l3_route_ipv6_s_common_init(second_unit, 0, &route_ipv6_structure, route2, mask, vrf, 0, fwd_only_flags, egress_intf[i%MAX_PORTS]);
                            rc = sand_utils_l3_route_ipv6_add(second_unit, &route_ipv6_structure);
                        }
                    } else {/*i == 3*/
                        if (lpm_raw_data_enabled) {
                            rc = add_route_ipv6_raw(second_unit, route3, mask, vrf, lpm_payload);
                        } else {
                            sand_utils_l3_route_ipv6_s_common_init(second_unit, 0, &route_ipv6_structure, route3, mask, vrf, 0, fwd_only_flags, egress_intf[i%MAX_PORTS]);
                            rc = sand_utils_l3_route_ipv6_add(second_unit, &route_ipv6_structure);
                        }
                    }
                }
            }
            if (rc != BCM_E_NONE) {
                printf("Error. add_route_ipv6 failed for route%d: $rc\n", i, rc);
                return BCM_E_FAIL;
            }
        }

        if (add_ipv6_uc_routes==1) {
            if (i == 0) {
                printf("route: "); ipv6_print(route0);
            } else if (i == 1) {
                printf("route: "); ipv6_print(route1);
            } else if (i == 2) {
                printf("route: "); ipv6_print(route2);
            } else {/* (i == 3) */
               printf("route: "); ipv6_print(route3);
            }
            if (i < 4) {
                printf("                                                ");
                printf("mask: "); ipv6_print(mask); printf("    vrf: %d\n", vrf);
            }
        }
        if  (i == 4) { /*i == 4 MC*/
            printf("route"); ipv6_print(route4);
        }
        if  (i == 5) {/*i == 5 MC*/
            printf("route"); ipv6_print(route5);
        }
    } /* for loop*/


    if (fall2bridge_ive_enable) {
        rc = l3_ip6_fall2bridge_ive(unit, in_port, in_vlan);
        if (rc != BCM_E_NONE)
        {
            printf("Error, l3_ip6_fall2bridge_ive\n");
            return rc;
        }
    }

    return 0;
}


/*
 * Test Scenario
 *     semantic test of IPv6 route with prefix length from 0 to 128
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint
 * ipv6_route_prefix_sem(0);
 * exit;
 */

int l3routeentries = 128;
int l3routecounter;
int l3routelistcheck[128];
bcm_l3_route_t l3routelist[128];

int
ipv6_route_prefix_sem_traverse_cb (
    int unit,
    int i,
    bcm_l3_route_t *info,
    void *data)
{
    bcm_l3_route_t l3route;
    bcm_ip6_t address;
    /* using the intf as an index */
    int index = info->l3a_intf - 0x20001000;

    l3route = l3routelist[index];

    ipv6_and_mask(address, l3route.l3a_ip6_net, l3route.l3a_ip6_mask);

    if(l3routelistcheck[index] != 0) {
        printf("Error: index=%d, 2nd check list hit\n", index);
        return BCM_E_FAIL;
    }
    else {
        l3routelistcheck[index] = 1;
    }
    if(sal_memcmp(info->l3a_ip6_net, address, 16)) {
        printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_net);
        printf(", expected=");
        ipv6_print(address);
        printf("\n");
        return BCM_E_FAIL;
    }
    if(sal_memcmp(info->l3a_ip6_mask, l3route.l3a_ip6_mask, 16)) {
        printf("Error: index=%d, ip6_mask=", index);
        ipv6_print(info->l3a_ip6_mask);
        printf(", expected=");
        ipv6_print(l3route.l3a_ip6_mask);
        printf("\n");
        return BCM_E_FAIL;
    }
    if(info->l3a_vrf != l3route.l3a_vrf) {
        printf("Error: index=%d, vrf=%d. VRF matching error, expected=%d\n", index, info->l3a_vrf, l3route.l3a_vrf);
        return BCM_E_FAIL;
    }
    if(info->l3a_intf != l3route.l3a_intf) {
        printf("Error: index=%d, info->l3a_intf=0x%08x. fec matching error, expected=0x%08x\n", index, info->l3a_intf, l3route.l3a_intf);
        return BCM_E_FAIL;
    }

    l3routecounter++;

    return BCM_E_NONE;
}

int ipv6_route_prefix_sem (
    int unit)
{
    int i, prefix, rv = 0;
    int vrf = 100;
    uint32 fec_route = 0x20001000;
    bcm_ip6_t route = { 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5 };
    bcm_l3_route_t l3route;
    uint32 fwd_rpf_only_flags[2] = { 0, 0 };
    int loop_index;
    int nof_loops = 1;

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        nof_loops = 2;
        fwd_rpf_only_flags[0] = BCM_L3_FLAGS2_FWD_ONLY;
        fwd_rpf_only_flags[1] = BCM_L3_FLAGS2_RPF_ONLY;
    }

    for(loop_index = 0; loop_index < nof_loops; loop_index++) {

        l3routecounter = 0;
        for(i = 0; i < l3routeentries; i++) {
            l3routelistcheck[i] = 0;
            bcm_l3_route_t_init(&l3routelist[i]);
        }

        printf("entries add - start\n");

        bcm_l3_route_t_init(&l3route);
        l3route.l3a_flags = BCM_L3_IP6;
        l3route.l3a_flags2 = fwd_rpf_only_flags[loop_index];
        sal_memcpy(l3route.l3a_ip6_net, route, 16);
        l3route.l3a_vrf = vrf;
        l3route.l3a_intf = fec_route;

        for(i = 0, prefix = 127; prefix >= 0; prefix--, i++) {
            ipv6_create_mask(l3route.l3a_ip6_mask, prefix);

            rv = bcm_l3_route_add(unit, &l3route);
            if(rv != BCM_E_NONE) {
                 printf("Error, bcm_l3_route_add Failed, prefix %d\n", prefix);
                 return rv;
            }
            l3routelist[i] = l3route;
            l3route.l3a_intf++;
        }
        printf("entries add - end\n");

        printf("entries get - start\n");

        bcm_l3_route_t_init(&l3route);
        l3route.l3a_flags = BCM_L3_IP6;
        l3route.l3a_flags2 = fwd_rpf_only_flags[loop_index];
        sal_memcpy(l3route.l3a_ip6_net, route, 16);
        l3route.l3a_vrf = vrf;

        for(i = 0, prefix = 127; prefix >= 0; prefix--, i++) {
            ipv6_create_mask(l3route.l3a_ip6_mask, prefix);

            rv = bcm_l3_route_get(unit, &l3route);
            if(rv != BCM_E_NONE) {
                 printf("Error, bcm_l3_route_get Failed, prefix %d\n", prefix);
                 return rv;
            }
            if(l3route.l3a_intf != l3routelist[i].l3a_intf) {
                 printf("Error, route: fec Failed, expected: 0x%08x, actual: 0x%08x\nprefix %d\n", fec_route, l3route.l3a_intf, prefix);
                 return rv;
            }
        }
        printf("entries get - end\n");

        printf("entries traverse - start\n");

        rv = bcm_l3_route_traverse(unit, BCM_L3_IP6, 0, 0, ipv6_route_prefix_sem_traverse_cb, NULL);
        if(rv != BCM_E_NONE) {
            printf("Error, bcm_l3_route_traverse\n");
            return rv;
        }

        if(l3routecounter != l3routeentries) {
            printf("Error, traverse entries counter mismatch, actual %d, expected %d\n", l3routecounter, l3routeentries);
            return -1;
        }
        for(i = 0; i < l3routeentries; i++) {
            if(l3routelistcheck[i] != 1) {
                print("Error, traverse hit check mismatch index %d\n", i);
                return -1;
            }
        }
        printf("entries traverse - end\n");

        printf("entries delete - start\n");

        bcm_l3_route_t_init(&l3route);
        l3route.l3a_flags = BCM_L3_IP6;
        l3route.l3a_flags2 = fwd_rpf_only_flags[loop_index];
        sal_memcpy(l3route.l3a_ip6_net, route, 16);
        l3route.l3a_vrf = vrf;

        for(prefix = 127; prefix >= 0; prefix--) {
            ipv6_create_mask(l3route.l3a_ip6_mask, prefix);

            rv = bcm_l3_route_delete(unit, &l3route);
            if(rv != BCM_E_NONE) {
                 printf("Error, bcm_l3_route_delete Failed, prefix %d\n", prefix);
                 return rv;
            }
        }
        printf("entries delete - end\n");
    }

    return BCM_E_NONE;
}

/*
 * modify l3 interface ingress for mtu check
 *  Parmeters:
 *  - mtu - maximal transmission unit
 *  - out_vlan - out vlan id
 *  Note:
 *  - On JER1, mtu size is set via bcm_l3_intf_create
 *  - On JR2, mtu size is set via bcm_rx_mtu_set
 */
int
modify_l3_interface_ingress (
    int unit,
    int mtu,
    int out_vlan)
{
    int CINT_NO_FLAGS = 0;
    int rc;
    bcm_l3_intf_t l3if, l3if_ori;
    bcm_switch_control_key_t  mtu_cfg_type;
    bcm_switch_control_info_t mtu_cfg_info;
    int ipv4_layer_protocol;
    int ipv6_layer_protocol;
    if (is_device_or_above(unit, JERICHO2))
    {
        dnx_dbal_fields_enum_value_get(unit, "LAYER_TYPES", "IPV4", &ipv4_layer_protocol);
        dnx_dbal_fields_enum_value_get(unit, "LAYER_TYPES", "IPV6", &ipv6_layer_protocol);
    }
    else
    {
        ipv4_layer_protocol = 2;
        ipv6_layer_protocol = 3;
    }

    int compressed_fwd_layer_type = 2;

    if (is_device_or_above(unit, JERICHO2)) {
        /* Create a compressed layer type for ipv4 */
        mtu_cfg_type.type = bcmSwitchLinkLayerMtuFilter;
        mtu_cfg_type.index = ipv4_layer_protocol;
        mtu_cfg_info.value = compressed_fwd_layer_type;
        rc = bcm_switch_control_indexed_set(unit, mtu_cfg_type, mtu_cfg_info);
        if(rc != BCM_E_NONE) {
            return rc;
        }

        /* Create a compressed layer type for ipv6 */
        mtu_cfg_type.index = ipv6_layer_protocol;
        mtu_cfg_info.value = compressed_fwd_layer_type;
        rc = bcm_switch_control_indexed_set(unit, mtu_cfg_type, mtu_cfg_info);
        if(rc != BCM_E_NONE) {
            return rc;
        }

        /* Set etpp mtu for rif */
        rc =  rc = mtu_check_etpp_rif_set(unit, out_vlan, compressed_fwd_layer_type, mtu, 1/*is_set*/);
        if(rc != BCM_E_NONE) {
            return rc;
        }
    } else {
        bcm_l3_intf_t_init(&l3if);
        /* before creating L3 INTF, check whether L3 INTF already exists*/
        bcm_l3_intf_t_init(&l3if_ori);
        l3if_ori.l3a_intf_id = out_vlan;
        rc = bcm_l3_intf_get(unit, &l3if_ori);
        if (rc == BCM_E_NONE) {
            /* if L3 INTF already exists, replace it*/
            l3if.l3a_flags = CINT_NO_FLAGS | BCM_L3_REPLACE | BCM_L3_WITH_ID;
            l3if.l3a_vid = l3if.l3a_intf_id = out_vlan;
            sal_memcpy(l3if.l3a_mac_addr, l3if_ori.l3a_mac_addr, 6);
        }
        else {
            return rc;
        }

        l3if.l3a_mtu = mtu;

        rc = bcm_l3_intf_create(unit, l3if);
        if (rc != BCM_E_NONE) {
            printf ("bcm_l3_intf_create failed: %d \n", rc);
        }
    }

    return rc;
}


/*
 * Purpose:
 *   Pmf parse the raw payload result from the LPM (KAPS).
 *   Then replace input payload ('kaps_result' with 'redir_kaps_result'
 *   This is done by adding PMF rule into TCAM to parse the result from the KAPS
 *
 * Usage:
 * Add an entry to the LPM with api: "bcm_l3_route_add()". use the flag "BCM_L3_FLAGS2_RAW_ENTRY"
 * Call this procedure to change the value extracted from LPM.
 * See example in cint_ip_route_basic.c; dnx_basic_example_inner_redirect()
 */
field_util_fg_t fg_t_kaps_raw_data;
int dnx_kaps_raw_data_redirect(
    int unit,
    int kaps_result,
    int is_redir_kaps_or_port,
    int redir_kaps_result,
    int redir_port)
{
    bcm_field_qualify_t qualifiers_set[1];
    bcm_field_qualify_attach_info_t qualify_attach_info[1];
    field_util_qual_values_t qual_datas[1];
    field_util_qual_values_t mask_datas[1];
    bcm_field_action_t actions_set[1];
    int act_datas[1];
    int nof_quals, nof_actions;
    bcm_field_group_t fg_id;
    field_util_fg_t fg_t_1;
    bcm_field_stage_t stage;
    int ii;
    int rv;
    field_util_entry_info_t ent_info_1;
    char *proc_name;

    proc_name = "dnx_kaps_raw_data_redirect";
    printf("%s(): Enter. kaps_result 0x%08X redir_kaps_result 0x%08X\r\n",proc_name, kaps_result, redir_kaps_result);

    fg_id = -1; /* SDK allocate field group id */
    nof_quals = 1;
    nof_actions = 1;
    stage = bcmFieldStageIngressPMF1;
    qualifiers_set[0] = bcmFieldQualifyDstPort;
    qualify_attach_info[0].input_type = bcmFieldInputTypeMetaData;
    /*
     * 'input_arg' and 'offset' are not meaningful for metaData.
     */
    qualify_attach_info[0].input_arg = 0;
    qualify_attach_info[0].offset = 0;
    sal_memset(qual_datas[0].value,0,sizeof(qual_datas[0].value));
    qual_datas[0].value[0] = kaps_result;
    sal_memset(mask_datas[0].value,0,sizeof(mask_datas[0].value));
    mask_datas[0].value[0] = 0x0FFFFF;

    if (is_redir_kaps_or_port) {
        actions_set[0] = bcmFieldActionForward;
        /*
         * Load with route of 'redirect' port.
         */
        act_datas[0] = redir_kaps_result;
    } else {
        actions_set[0] = bcmFieldActionRedirect;
        act_datas[0] = redir_port;;
    }

    printf("%s(): Initialize FG structure\r\n",proc_name);
    field_util_fg_t_init(unit, &fg_t_1);
    fg_t_1.fg_id = fg_id;
    fg_t_1.stage = stage;
    for (ii = 0; ii < nof_quals; ii++)
    {
        fg_t_1.bcm_qual[ii] = qualifiers_set[ii];
        fg_t_1.qual_info[ii] = qualify_attach_info[ii];
    }
    for (ii = 0; ii < nof_actions; ii++)
    {
        fg_t_1.bcm_actions[ii] = actions_set[ii];
    }
    /*
     * Create the FG with structure fg_t_1
     */
    rv = field_util_fg_add(unit, &fg_t_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), reported by field_util_fg_add()\n", proc_name, rv);
        return rv;
    }
    /*
     * Attaching the FG on default context
     */
    rv = field_util_fg_context_attach(unit, 0, &fg_t_1, BCM_FIELD_INVALID);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), reported by field_util_fg_context_attach()\n", proc_name, rv);
        return rv;
    }
    /*
     * Adding field entry with qualifier and action values
     */
    field_util_entry_info_init(unit, &ent_info_1);
    ent_info_1.fg_id = fg_t_1.fg_id;
    for (ii = 0; ii < nof_quals; ii++)
    {
        ent_info_1.bcm_qual[ii] = qualifiers_set[ii];
        ent_info_1.qual_values[ii] = qual_datas[ii];
        ent_info_1.qual_masks[ii] = mask_datas[ii];
    }
    for (ii = 0; ii < nof_actions; ii++)
    {
        ent_info_1.bcm_actions[ii] = actions_set[ii];
        ent_info_1.action_values[ii].value[0] = act_datas[ii];
    }
    rv = field_util_field_entry_add(unit, 0, &ent_info_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), reported by field_util_field_entry_add()\n", proc_name, rv);
        return rv;
    }
    /** Pass entry ID */
    entry_id_created = ent_info_1.entry_handle;

    fg_t_kaps_raw_data = fg_t_1;
    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

int
dnx_basic_example_raw_redirect(
    int unit,
    int in_port,
    int out_port,
    int redir_port,
    int modid,
    int kaps_result,
    int redir_kaps_result,
    int redir_encap_id)
{
    int rv;
    char *proc_name;

    proc_name = "dnx_basic_example_raw_redirect";
    printf("%s(): Enter. in_port %d out_port %d redir_port %d modid %d\r\n",proc_name, in_port, out_port, redir_port, modid);
    printf("==>  kaps_result 0x%08X redir_kaps_result 0x%08X redir_encap_id 0x%08X\r\n",kaps_result, redir_kaps_result, redir_encap_id);
    if (already_called)
    {
        return 0;
    }
    already_called = 1;
    rv = dnx_basic_example_inner_redirect(unit, in_port, out_port, redir_port, modid, kaps_result, redir_kaps_result,redir_encap_id);
    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/* 
 * Test Scenario
 *   Function to check push label on EEI in LEM
 *   check the ipv4 entry in LEM is returned as expected
 *
 * Expected :
 *   bcm_l3_host_find() should return the expected ipv4 entry.
 *
 * Note: 
 *   For Jr2, push label on EEI in LEM is available only in interop mode
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/cint_ip_route_scenarios.c
 * cint
 * verify_host_entry(0);
 * exit;
 */

int
verify_host_entry (
    int unit)
{
    int rv = BCM_E_NONE;
    int vrf = 100;
    int fecId;
    bcm_l3_intf_t intf;
    bcm_mac_t out_mac = {0x00, 0x45, 0x45, 0x45, 0x45, 0x00};
    bcm_l3_egress_t l3eg;
    bcm_l3_host_t l3host;
    bcm_l3_host_t l3hostfind;
    uint32 IPaddr = 0x22664501;
    int out_port = 202;
    int flags = 0;
    int label = 30;
    int eei=0;
    int mpls_port_push_profile = 2;

    /* Create EEDB entry*/
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, out_mac, 6); /* Packet will get out_mac when leaving the device*/

    l3eg.vlan = 100;                       /* Out vlan*/
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &fecId);

    l3eg.port = out_port;                  /* Out port */
    l3eg.intf = l3eg.vlan;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fecId);

    /* Add label details for Push label on EEI */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = IPaddr;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fecId;
    l3host.l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(eei, label, mpls_port_push_profile);
    BCM_FORWARD_ENCAP_ID_VAL_SET(l3host.encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI, BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, eei);
    rv = bcm_l3_host_add(unit, &l3host);

    bcm_l3_host_t_init(&l3hostfind);
    l3hostfind.l3a_ip_addr = IPaddr;
    l3hostfind.l3a_vrf = vrf;

    bcm_l3_host_find(unit,&l3hostfind);
    if (!(l3hostfind.l3a_flags & BCM_L3_ENCAP_SPACE_OPTIMIZED)) {
        printf("flag BCM_L3_ENCAP_SPACE_OPTIMIZED should be set\n");
        return 1;
    }
    if (l3hostfind.encap_id != l3host.encap_id) {
        printf("l3hostfind.encap_id is different than l3host.encap_id\n");
        printf("l3hostfind.encap_id=%d\n",l3hostfind.encap_id);
        printf("l3host.encap_id=%d\n",l3host.encap_id);
        return 1;
    }
    return 0;
}

