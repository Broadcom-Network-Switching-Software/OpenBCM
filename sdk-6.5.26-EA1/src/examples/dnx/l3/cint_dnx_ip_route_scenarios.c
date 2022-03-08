/* $Id: cint_dnx_ip_route_scenarios.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */


/*
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_scenarios.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint
 * ipv6_route_prefix(0,200,201);
 * exit;
 *
 * tx 1 psrc=200 data=0x000a00000b001100000000118100006486dd600000000021068012345678000000000000000087654321a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00000022000a00000b008100006486dd608000100014060012345678000000000000000087654321a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a53344556600a1a2a300b1b2b3500f01f56e2d000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
  * cint
 * ipv6_route_prefix_entry_delete(0,124);
 * exit;
 *
 */

/* debug prints */
int verbose = 1;

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

    dnx_utils_l3_arp_s l3_arp;
    dnx_utils_l3_fec_s l3_fec;
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
        dnx_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id, 0, 0, next_hop_mac, vlan);
        rv = dnx_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, dnx_utils_l3_arp_create: encap_id = %d, vsi = %d, err_id = %d\n", proc_name,
                   encap_id, vlan, rv);

        }

        /*
         * 6. Create FEC and set its properties
         */
        dnx_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        BCM_GPORT_LOCAL_SET(l3_fec.destination, out_port);
        l3_fec.tunnel_gport = vlan;
        l3_fec.tunnel_gport2 = encap_id;
        l3_fec.fec_id = fec;
        rv = dnx_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, dnx_utils_l3_fec_create: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
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
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_scenarios.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint
 * bcm_l3_ip6_uc_priority(0,200,201,202,203,0);
 * exit;
 *
 * tx 1 psrc=200 data=0x000000005510000090569736810007d086dd60000000002106800000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x000000006630000000005530810007da86dd600000000021067f0000000000000000000000000000012301001600350070000000db0700000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 *
 */

int NOF_INTERFACES = 4;

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

    dnx_utils_l3_port_s l3_port_structure;
    dnx_utils_l3_eth_rif_s eth_rif_structure;
    dnx_utils_l3_arp_s l3_arp;
    dnx_utils_l3_fec_s l3_fec;

    /*Set each interface parameters*/
    vlan[0] = 2000;
    mac_last_byte[0] = 0x10;
    for (i=1; i < NOF_INTERFACES; i++) {
        vlan[i] = vlan[i-1]+10;
        mac_last_byte[i] = mac_last_byte[i-1]+0x20;
    }
    dnx_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, port1, vlan[0]);
    rc = dnx_utils_l3_port_set(unit, &l3_port_structure);
    if (rc != BCM_E_NONE)
    {
        printf("Error, dnx_utils_l3_port_set\n");
        return rc;
    }
    dnx_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, port0, vlan[0]);
    rc = dnx_utils_l3_port_set(unit, &l3_port_structure);
    if (rc != BCM_E_NONE)
    {
        printf("Error, dnx_utils_l3_port_set\n");
        return rc;
    }
    dnx_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, port2, vlan[0]);
    rc = dnx_utils_l3_port_set(unit, &l3_port_structure);
    if (rc != BCM_E_NONE)
    {
        printf("Error, dnx_utils_l3_port_set\n");
        return rc;
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

        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan[i], 0, 0, mac_l3_ingress_add, vlan[i]);
        rc = dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, dnx_utils_l3_eth_rif_create\n");
        }
        ingress_intf[i] = eth_rif_structure.l3_rif;

        /*Create L3 egress*/
        mac_l3_engress_add[5] = mac_last_byte[i];
        /* create ARP and set its properties */
        dnx_utils_l3_arp_s_common_init(unit, 0, &l3_arp, 0, 0, encap_id[i], mac_l3_engress_add, vlan[i]);
        rc = dnx_utils_l3_arp_create(unit, &l3_arp);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rc;
        }
        encap_id[i] = l3_arp.l3eg_arp_id;

        /* create FEC and set its properties */
        dnx_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        BCM_GPORT_LOCAL_SET(l3_fec.destination, ports[i]);
        l3_fec.tunnel_gport = ingress_intf[i];
        l3_fec.tunnel_gport2 = encap_id[i];
        l3_fec.fec_id = egress_intf[i];
        l3_fec.allocation_flags = 0;
        rc= dnx_utils_l3_fec_create(unit, &l3_fec);
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
 *     semantic test of IPv6 route with prefix length from 0 to 128
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/examples/dnx/l3/cint_dnx_ip_route_scenarios.c
 * cint
 * ipv6_route_prefix_sem(0);
 * exit;
 *
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
  * Test Scenario
  *
  * ./bcm.user
  * cd ../../../../regress/bcm
  * cint ../../src/examples/dnx/utility/cint_dnx_utils_global.c
  * cint ../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  * cint ../../src/examples/dnx/utility/cint_dnx_utils_multi_device.c
  * cint ../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
  * cint ../../src/examples/dnx/field/cint_field_utils.c
  * cint ../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  * cint ../../src/examples/dnx/l3/cint_dnx_ip_route_scenarios.c
  * cint
  * mac_src_encap_example_dvapi(units_ids,1,1811939528,1811939529);
  * exit;
  *
  * /* IPv4oETH1 */
  * tx 1 psrc=200 data=0x000c000200000000070001008100000f080045000035000000008000b1c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
  *
  * /* Received packets on unit 0 should be: */
  * /* Source port: 0, Destination port: 0 */
  * /* Data: 0x00000000cd1d00000000cd1581000065080045000035000000007f00b2c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
  *
  */

 int src_encap_out_vlan_port_id;
 int
 ac_action_add (
     int unit,
     int out_vlan,
     int out_sysport)
 {
     int rv;
     int action_id;
     bcm_vlan_action_set_t action;
     bcm_vlan_port_t vlan_port;
     bcm_port_match_info_t port_match_info;
     bcm_vlan_translate_action_class_t action_class;

     bcm_vlan_port_t_init(&vlan_port);
     vlan_port.port = out_sysport;
     vlan_port.vsi = out_vlan;
     vlan_port.flags = BCM_VLAN_PORT_VLAN_TRANSLATION;
     vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
     vlan_port.flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
     rv = bcm_vlan_port_create(unit, &vlan_port);
     if (rv != BCM_E_NONE) {
         printf("Error, bcm_vlan_port_create\n");
     }

     src_encap_out_vlan_port_id = vlan_port.vlan_port_id;

     rv = vlan_translate_ive_eve_translation_set(unit, src_encap_out_vlan_port_id, 0x8100, 0, bcmVlanActionAdd,0,(out_vlan+1),0,2,0,0);
     if (rv != BCM_E_NONE) {
         printf("Error, vlan_translate_ive_eve_translation_set\n");
     }
     return rv;
 }

 int
 mac_src_encap_example_dvapi (
     int unit,
     int in_sysport,
     int out_sysport)
 {
     int rv;
     int i, host;
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
     dnx_utils_l3_eth_rif_s eth_rif;
     l3_egress_s l3_egress;
     dnx_utils_l3_route_ipv4_s route_ipv4;

     flags = 0;
     ing_intf_in = 0;

     rv = bcm_vlan_create(unit, in_vlan);
     if (rv != BCM_E_NONE) {
         printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
     }

     rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
     if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
         printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan);
         return rv;
     }

     dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif, in_vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, mac_address, vrf);
     rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif);
     if (rv != BCM_E_NONE)
     {
         printf("Error, dnx_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
     }
     ing_intf_in = eth_rif.l3_rif;

     /*** create egress router interface ***/
     ing_intf_out = 0;
     flags = 0;
     rv = bcm_vlan_create(unit, out_vlan);
     if (rv != BCM_E_NONE) {
         printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
     }
     rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
     if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
         printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
         return rv;
     }

     dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif, out_vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, mac_address, vrf);
     rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif);
     if (rv != BCM_E_NONE)
     {
         printf("Error, dnx_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
     }
     ing_intf_out = eth_rif.l3_rif;

     l3_egress.allocation_flags = 0;
     dnx_utils_l3_arp_s l3_arp;
     dnx_utils_l3_fec_s l3_fec;

     dnx_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[1], 0, BCM_L3_FLAGS2_SRC_MAC | BCM_L3_FLAGS2_EGRESS_STAT_ENABLE, next_hop_mac, out_vlan);
     l3_arp.src_mac_addr = custom_mac;
     rv = dnx_utils_l3_arp_create(unit, &l3_arp);
     if (rv != BCM_E_NONE)
     {
         printf("Error, create AC egress object ARP only\n");
         return rv;
     }
     encap_id[1] = l3_arp.l3eg_arp_id;;

     /* Create FEC and set its properties */
     dnx_utils_l3_fec_s_common_init(unit, 0/*allocation_flags*/, 0, l3_fec, fec[1], 0/*l3_flags*/, 0/*l3_flags2*/, out_sysport, ing_intf_out, encap_id[1]);
     rv = dnx_utils_l3_fec_create(unit, &l3_fec);
     if (rv != BCM_E_NONE)
     {
         printf("Error, create egress object FEC only\n");
         return rv;
     }
     fec[1] = l3_fec.l3eg_fec_id;

     rv = ac_action_add(unit, out_vlan, out_sysport);
     if (rv != BCM_E_NONE) {
         printf("Error, ac_action_add\n");
         return rv;
     }

     /*** add route point to FEC2 ***/
     route = 0x7fffff00;
     mask  = 0xfffffff0;
     dnx_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4, route, mask, vrf, 0, 0, fec[1]);
     rv = dnx_utils_l3_route_ipv4_add(unit, &route_ipv4);
     if (rv != BCM_E_NONE) {
         printf("Error, in function dnx_utils_l3_route_ipv4_add, \n");
         return rv;
     }

     return rv;
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
 bcm_field_entry_t entry_id_created;
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


