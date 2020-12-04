/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * Cint VRRP Setup example code
 * 
 *
 * Run script:
 *
 * cd ../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cd ../../../src/examples/dpp/utility/cint_utils_vlan.c
 * cd ../../../src/examples/dpp/utility/cint_utils_l3.c
 * cd ../../../src/examples/sand/cint_ip_route.c
 * cd ../../../src/examples/sand/cint_l3_vrrp.c
 * cint
 * l3_vrrp_traffic_example(unit, <in_port>, <out_port>, <max_nof_vlans>, <ipv6_distinct>);
 * 
 * for Soc_petra-B:
 * max_nof_vlans must be 4096 (and therefore ipv6_distinct must be 0).
 * 
 * In ARADPLUS and above:
 * 
 * There are two relevant max_nof_vlans in aradplus. The first is 4096 which allows you to have a total of 8 different vrids between 0-255.
 * The second is 256 which allows you to have 16 different vrids between 0-255 and works identically to vrrp in arad.
 */

int fec;

/*  After adding vrrp mapping, use this function to map all traffic that
 *  has recieved on in_port, been l2 terminated and has vlan == in_vlan.
 *  Those packets will be forwarded to out_port.
 */
int create_traffic_mapping(int unit, int in_port, int out_port, int in_vlan, int ipv6_distinct){
    int rv;
    int ing_intf_in;
    int ing_intf_out; 
    int out_vlan = 100;
    int vrf = 0;
    int encap_id; 
    int route;
    int mask; 
    bcm_ip6_t route6;
    bcm_ip6_t mask6;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac */
    int vrrp_flags;


    /* create ingress router interface */
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }

    if (ipv6_distinct) {
        rv = vlan__open_vlan_per_mc(unit, in_vlan - 1, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        }
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_vlan_gport_add\n");
            return rv;
        }
    }
    create_l3_intf_s intf;
    sal_memset(&intf, 0, sizeof(intf));
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;        
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    if (ipv6_distinct) {
        intf.vsi = in_vlan  - 1;
        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_in = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }
    
    /* create egress router interface */
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", in_port, out_vlan);
      return rv;
    }
    
    intf.vsi = out_vlan;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

   /* create egress object (FEC) */
    create_l3_egress_s l3eg2;
    sal_memcpy(l3eg2.next_hop_mac_addr, next_hop_mac , 6);
    l3eg2.out_tunnel_or_rif = ing_intf_out;
    l3eg2.out_gport = out_port;
    l3eg2.vlan = out_vlan;
    l3eg2.fec_id = fec;
    l3eg2.arp_encap_id = encap_id;

    rv = l3__egress__create(unit,&l3eg2);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3eg2.fec_id;
    encap_id = l3eg2.arp_encap_id;

    printf("created FEC-id =0x%08x, \n", fec);
    printf("next hop mac at encap-id %08x, \n", encap_id);

    /* add IPV4 route point to FEC -> out_port */
    route = 0x7fffff00;
    mask  = 0xfffffff0; 

    rv = add_route(unit, route, mask , vrf, fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_route \n");
        return rv;
    }
    print_route("add entry ", route,mask,vrf);
    printf("---> egress-object=0x%08x, port=%d, \n", fec, out_port);

    /* add IPV6 route point to FEC -> out_port */
    if (is_device_or_above(unit, JERICHO2)) {
        mask6[0]= 0xff;
        mask6[1]= 0xff;
        mask6[2]= 0xff;
        mask6[3]= 0xff;
        mask6[4]= 0xff;
        mask6[5]= 0xff;
        mask6[6] = 0xff;
        mask6[7] = 0xff;
        mask6[8] = 0;
        mask6[9] = 0;
        mask6[10] = 0;
        mask6[11] = 0;
        mask6[12] = 0;
        mask6[13] = 0;
        mask6[14] = 0;
        mask6[15] = 0;
    } else {
        mask6[15]= 0xff;
        mask6[14]= 0xff;
        mask6[13]= 0xff;
        mask6[12]= 0xff;
        mask6[11]= 0xff;
        mask6[10]= 0xff;
        mask6[9] = 0xff;
        mask6[8] = 0xff;
        mask6[7] = 0;
        mask6[6] = 0;
        mask6[5] = 0;
        mask6[4] = 0;
        mask6[3] = 0;
        mask6[2] = 0;
        mask6[1] = 0;
        mask6[0] = 0;
    }

    route6[15]= 0; /* LSB */
    route6[14]= 0;
    route6[13]= 0;
    route6[12]= 0;
    route6[11]= 0x7;
    route6[10]= 0xdb;
    route6[9] = 0;
    route6[8] = 0;
    route6[7] = 0;
    route6[6] = 0x70;
    route6[5] = 0;
    route6[4] = 0x35;
    route6[3] = 0;
    route6[2] = 0x16;
    route6[1] = 0;
    route6[0] = 0x1; /* MSB */

    rv = add_route_ip6(unit, route6, mask6, vrf, fec);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route_ip6 \n");
        return rv;
    }

    printf("l3_vrrp_test_PASS\n");
    return rv;
}

/* 
 * Use this function to configure l3 vrrp traffic on the device.  
 *  
 * max_nof_vlans: how many vlans are supported.
 *                possible values: 4096, 2048, 1024, 512, 256
 *                in aradplus, possible values: 4096, 256
 *                max_nof_vlans = 4096/2048 - ipv6 cannot be supported at all
 *                in aradplus ipv6 can be used with all max_nof_vlans.
 *                SOC property: l3_vrrp_max_vid
 *  
 * ipv6_distinct: relevent only for max_nof_vlans = 1024/512/256 
 *                in aradplus, relevant for all mox_nof_vlans. 
 *                if set: mode is ipv6 distinct, else: mode is shared between ipv4 and ipv6
 *                SOC property: l3_vrrp_ipv6_distinct
 *  
 *  
 * in petrab and arad, vrid=0x03 
 * in aradplus, vrid = 0x6b 
 *  
 * IPV4 packet to send from in_port:  
 *  - vlan = in_vlan (= max_nof_vlans - 1, or 4095, if above 4096).
 *  - DA = {0x00, 0x00, 0x5e, 0x00, 0x01, vrid}
 *  - IPV4 DIP = 0x7fffff00 (127.255.255.0)
 *  packet will be routed to out_port
 *  there will be termination (although DA is not MyMac) based on VRID
 *  
 *  
 * IPV6 packet to send from in_port:  
 *  - vlan = in_vlan (= max_nof_vlans - 1, or 4095, if above 4096).
 *  - DA =  if ipv6_distinct {0x00, 0x00, 0x5e, 0x00, 0x02, vrid}, otherwise {0x00, 0x00, 0x5e, 0x00, 0x01, vrid}
 *          
 *  - IPV6 DIP = any
 *  there will be termination based on VRID
 *  
 * if ipv6_distinct - the following should work as well: 
 * 1) same IPV4 packet with: 
 *    - DA = {0x00, 0x00, 0x5e, 0x00, 0x01, vrid + 1}
 *    - vlan = in_vlan - 1
 * 2) same IPV6 packet with: 
 *    - DA = {0x00, 0x00, 0x5e, 0x00, 0x02, vrid + 2}
 *    - vlan = in_vlan - 1
 *  in both cases there will be termination based on VRID
 */
int l3_vrrp_traffic_example(int unit, int in_port, int out_port, int max_nof_vlans, int ipv6_distinct){
    int rv;
    int vrrp_flags;
    int in_vlan = max_nof_vlans - 1;
    int vrid;

    /* Aradplus can handle a wider range of vrids. */
    if (is_device_or_above(unit,ARAD_PLUS)) {
        vrid = 0x6b;
    } else {
        vrid = 0x03;
    }

    /* In jericho, we can support more than 4096 vlans, but we have to define them first. */
    if (in_vlan > BCM_VLAN_MAX) {
        if (!is_device_or_above(unit, JERICHO)) {
            /* Illegal */
            printf("Illegal max_nof_vlans: %d\n", max_nof_vlans);
            return BCM_E_PARAM;
        }

        /* Create a vlan port, which would map the port x vlan 4095 to the max_num_vlans -1 */
        rv = vlan__vlan_port_vsi_create(unit, in_vlan, in_port, BCM_VLAN_MAX, BCM_VLAN_INVALID);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan__vlan_port_vsi_create\n");
            print rv;
            return rv;
        }

        /* Create a vlan port, which would map the port x vlan 4094 to the max_num_vlans -2 */
        if (ipv6_distinct) {
            rv = vlan__vlan_port_vsi_create(unit, in_vlan - 1, in_port, BCM_VLAN_MAX  - 1, BCM_VLAN_INVALID);
            if (rv != BCM_E_NONE) {
                printf("Error, in vlan__vlan_port_vsi_create\n");
                print rv;
                return rv;
            }
        }
    }

    if (ipv6_distinct) {
        /* add VRRP mac_lsb for in_vlan, once for IPV6 and once for IPV4
           for IPV4 only add also VRRP mac_lsb+1 for in_vlan - 1
           for IPV6 only add also VRRP mac_lsb+2 for in_vlan - 1 */

        vrrp_flags = BCM_L3_VRRP_IPV4;
        rv = bcm_l3_vrrp_config_add(unit, vrrp_flags, in_vlan, vrid);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_l3_vrrp_config_add with vlan %d and vrid %d\n", in_vlan, vrid);
            return BCM_E_FAIL;
        }
        rv = bcm_l3_vrrp_config_add(unit, vrrp_flags, in_vlan - 1, vrid+1);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_l3_vrrp_config_add with vlan %d and vrid %d\n", in_vlan, vrid+1);
            return BCM_E_FAIL;
        }

        vrrp_flags = BCM_L3_VRRP_IPV6;
        rv = bcm_l3_vrrp_config_add(unit, vrrp_flags, in_vlan, vrid);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_l3_vrrp_config_add with vlan %d and vrid %d\n", in_vlan, vrid);
            return BCM_E_FAIL;
        }
        rv = bcm_l3_vrrp_config_add(unit, vrrp_flags, in_vlan - 1, vrid+2);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_l3_vrrp_config_add with vlan %d and vrid %d\n", in_vlan, vrid+2);
            return BCM_E_FAIL;
        }
    }
    else { /* VRRP is shared between IPV4 and IPV6 */
        /* set VRRP mac_lsb for in_vlan for IPV4 and IPV6 */
        vrrp_flags = BCM_L3_VRRP_IPV4 | BCM_L3_VRRP_IPV6;
        rv = bcm_l3_vrrp_config_add(unit, vrrp_flags, in_vlan, vrid);
        if (rv != BCM_E_NONE) {
            printf("Error. bcm_l3_vrrp_config_add with vlan %d and vrid %d\n", in_vlan, vrid);
            return BCM_E_FAIL;
        }
    }

    rv = create_traffic_mapping(unit, in_port, out_port, in_vlan, ipv6_distinct);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_traffic_mapping\n");
        return rv;
    }

    return rv;
}

int add_route_ip6(int unit, bcm_ip6_t *addr, bcm_ip6_t *mask, int vrf, int intf) {

  int rc;
  bcm_l3_route_t l3rt;

  bcm_l3_route_t_init(&l3rt);

  l3rt.l3a_flags = BCM_L3_IP6;
  sal_memcpy(&(l3rt.l3a_ip6_net),(addr),16);
  sal_memcpy(&(l3rt.l3a_ip6_mask),(mask),16);
  l3rt.l3a_vrf = vrf;
  l3rt.l3a_intf = intf;
  l3rt.l3a_modid = 0;
  l3rt.l3a_port_tgid = 0;

  rc = bcm_l3_route_add(unit, l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %d \n", rc);
  }
  
  bcm_l3_route_t_init(&l3rt);
  l3rt.l3a_flags = BCM_L3_IP6;
  l3rt.l3a_vrf = vrf;
  sal_memcpy(&(l3rt.l3a_ip6_net),(addr),16);
  sal_memcpy(&(l3rt.l3a_ip6_mask),(mask),16);
  rc = bcm_l3_route_get(unit, &l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_get failed: %d \n", rc);
  }
  print l3rt;
  
  return rc;
}


