/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IPv6 Example script
 */

/* 
the cint creates four UC IPV6 entries and one MC IPV6 entry. 
 
run: 
cint ../sand/utility/cint_sand_utils_global.c 
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c 
cint 
print qos_map_l3(unit); 
print bcm_l3_ip6(unit, <second_unit>, <in_port>, <out_port>);
 
for one device: call bcm_l3_ip6() with second_unit<0
 
for two devices: make sure that master device is unit 0 with modid 0 and slave device is unit 1 with modid 1

traffic example for two devices: 
 
    run ethernet header with DA 0:0:0:0:55:34 (for UC) and vlan tag id 17
    and IPV6 header with DIP according to the following:
    
    device 0:
    ---------
      <in_port>:    route  01001600350070000000db0700000000 : 100:1600:3500:7000:0:db07:0:0  
                    mask   0000000000000000ffffffffffffffff : 0:0:0:0:ffff:ffff:ffff:ffff
 
                    route  01001600350064000000db0700000000 : 100:1600:3500:6400:0:db07:0:0 
                    mask   0000000000000000fcffffffffffffff : 0:0:0:0:fcff:ffff:ffff:ffff
                    
                    
 
                  
UC packets from device 0 will be forwrded to <out_port> on device 1.
UC packets from device 1 will be forwrded to <out_port> on device 0.
MC packet from both devices will be forwrded to device 0 and to device 1 on ports <out_port>,<out_port+1>.
 
UC packets will arrive at out_port with: 
    ethernet header with DA 0:0:0:0:cd:1d, SA 0:0:0:0:55:34 and vlan tag id 18 
MC packets will arrive at out_port with: 
    ethernet header with DA 0:0:0:0:cd:1d, SA 33:33:3:2:1:0 and vlan tag id 18 
    IPV6 header with same DIP and hop_limit decremented

traffic example for single device:

run same packets from <in_port>. All 4 types of UC packets will arrive at <out_port>.
MC packets will arrive at ports <out_port>,<out_port+1> and will not arrive at <in_port>, because of same-interface filter. 
 
-------------------------------------------------------------------------------- 
 
The cint can be also used to test RIF TTL thershold:
In this example, same out_rif (=18) is created, but this time its TTL threshold is set as well. 
If an IPV6 MC packet arrives with a hop-limit that is lower than the threshold, the packet will be dropped at egress.  
calling sequence: 
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c 
cint 
print qos_map_l3(unit); 
print bcm_l3_ip6_with_ttl(unit, -1, <in_port>, <out_port>, <ttl_threshold>); 
 
 
The cint can be also used to test mtu by calling modify_l3_interface_ingress(int unit, int mtu, int out_vlan)
after bcm_l3_ip6. In this example out_vlan is set to 18, so to disable mtu use the following sequence:
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c 
cint 
print qos_map_l3(unit); 
print bcm_l3_ip6(unit,-1,13,14);
print modify_l3_interface_ingress(unit, 0, 18);
where out-vlan 18 is the outgoing interface 

*/
int MAX_PORTS=2;
int ingress_intf[MAX_PORTS];
int egress_intf[MAX_PORTS];

bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34}; /* incoming DA (myMAc) */

int default_vrf=0;
int default_intf=0;


/*
 * modify l3 interface ingress for rpf check
 *  Parmeters:
 *  - unit identified
 *  - rpf_mode - The In-RIF rpf mode
 *  - Interface id
 */
int modify_l3_interface_ingress_rpf(int unit, bcm_l3_ingress_urpf_mode_t rpf_mode, int intf) {

  int CINT_NO_FLAGS = 0;
  int rc;
  bcm_l3_intf_t l3if, l3if_ori;
  bcm_l3_ingress_t l3_ing_if; 

  bcm_l3_intf_t_init(&l3if);

  /* before creating L3 INTF, check whether L3 INTF already exists*/
  bcm_l3_intf_t_init(&l3if_ori);
  l3if_ori.l3a_intf_id = intf;
  l3if.l3a_intf_id = intf;
  /* rc = bcm_l3_intf_get(unit, &l3if_ori); */
  rc = bcm_l3_intf_get(unit, &l3if);
  if (rc == BCM_E_NONE) {
      /* if L3 INTF already exists, replace it*/
      printf("modify_l3_interface_ingress_rpf found l3if, vrf %d, inf_id %d, flags %d \n", l3if.l3a_vrf, l3if.l3a_intf_id, l3if.l3a_flags);
      l3if.l3a_flags = BCM_L3_REPLACE | BCM_L3_WITH_ID | BCM_L3_RPF;
  }
  else {
      printf("ERROR: Could not modify_l3_interface_ingress_rpf, did not find ... NOT EXIT\n");
      return rc;
  }

  /* Get the existing In-RIF */
  bcm_l3_ingress_t_init(&l3_ing_if);

  rc = bcm_l3_ingress_get(unit, intf, &l3_ing_if);
  if (rc != BCM_E_NONE) {
      printf("ERROR: modify_l3_interface_ingress_rpf, Could not find a In-RIF\n");
      return rc;
  }

  /* Update In-RIF RPF Mode */
  l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_REPLACE; /* must, as we update exist RIF */
  l3_ing_if.urpf_mode = rpf_mode;
  l3_ing_if.vrf = 3;
 
  rc = bcm_l3_ingress_create(unit, &l3_ing_if, l3if.l3a_intf_id);
  if (rc != BCM_E_NONE) {
      printf("ERROR: modify_l3_interface_ingress_rpf, ubable to set In-RIF values %d\n",l3if.l3a_intf_id);
      return rc;
  }
  /* print l3_ing_if; */

  return rc;
}

/*
 * delete l3 interface
 */
int delete_l3_ingress(int unit) {

  int rc;

  rc = bcm_l3_intf_delete_all(unit);

  return rc;
}



/************************************
 * Utilities
 */
void print_host(char *type, bcm_ip6_t host, int vrf)
{
  int a,b,c,d;

  printf("%s  vrf:%d   ", type, vrf);
  print_ipv6_addr("host", host);
}

void print_ipv6_addr(char *type, bcm_ip6_t a)
{
  printf("%s = ",type);
  printf("%x:", ((a[0] << 8) | a[1]));
  printf("%x:", ((a[2] << 8) | a[3]));
  printf("%x:", ((a[4] << 8) | a[5]));
  printf("%x:", ((a[6] << 8) | a[7]));
  printf("%x:", ((a[8] << 8) | a[9]));
  printf("%x:", ((a[10] << 8) | a[11]));
  printf("%x:", ((a[12] << 8) | a[13]));
  printf("%x\n", ((a[14] << 8) | a[15]));
}

void print_mac_addr(bcm_mac_t mac)
{
    printf ("mac %02x:%02x:%02x:%02x:%02x:%02x \n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
/*
 * Add Route
 */
int add_route_ip6(int unit, bcm_ip6_t addr, bcm_ip6_t mask, int vrf, int intf) {

  int rc;

  /* Use new add route using field processor */
  rc =  field_ipv6_entry_add (unit, group, addr, mask, vrf, intf);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_get failed: %d \n", rc);
  }
  
  return rc;
}


/*
 * Add IPV6 Host
 */
int add_ipv6_host(int unit, bcm_ip6_t *addr, int vrf, int fec, int intf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags =  BCM_L3_IP6;
  sal_memcpy(&(l3host.l3a_ip6_addr),(addr),16);
  l3host.l3a_vrf = vrf;
  /* dest is FEC + OutLif */
  l3host.l3a_intf = fec; /* fec */
  l3host.encap_id = intf; /* outlif */
  l3host.l3a_modid = 0;
  l3host.l3a_port_tgid = 0;

  rc = bcm_l3_host_add(unit, l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %d \n", rc);
  }
  return rc;
}


/*
 * delete l3 interface
 */
int delete_l3_ingress(int unit) {

  int rc;

  rc = bcm_l3_intf_delete_all(unit);

  return rc;
}


/************************************
 * Utilities
 */

void print_mac_addr(bcm_mac_t mac)
{
    printf ("mac %02x:%02x:%02x:%02x:%02x:%02x \n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
/*
 * Add Route
 */
int add_route_ip6(int unit, bcm_ip6_t addr, bcm_ip6_t mask, int vrf, int intf) {

  int rc;

  /* Use new add route using field processor */
  rc =  field_ipv6_entry_add (unit, group, addr, mask, vrf, intf);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_get failed: %d \n", rc);
  }
  
  return rc;
}


int bcm_l3_ip6(int unit, int second_unit ,int in_port, int out_port) {

    return bcm_l3_ip6_with_ttl(unit, second_unit, in_port, out_port, 255);
}

int bcm_l3_ip6_with_ttl(int unit, int second_unit, int in_port, int out_port, int ttl_threshold) {

  int CINT_NO_FLAGS = 0;
  int vrf = 3;
  int rc;
  int in_vlan = 17;
  int out_vlan = 18;
  int nof_mc_members = 2;
  int ipmc_index[MAX_PORTS] = {6010, 6011};

  int encap_id[MAX_PORTS];
  bcm_multicast_t mc_id[MAX_PORTS];

  /* used for multi device: */
  int second_ingress_intf[MAX_PORTS];
  int ingress_port_list[MAX_PORTS];
  int egress_port_list[MAX_PORTS];

  bcm_if_t cud;
  int trap_id, port_ndx;
  bcm_rx_trap_config_t trap_config;

  bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */

  create_l3_intf_s intf;

  /*bshell(unit, "dbm bcm +l3 +api +verbose");*/

  if (second_unit<0)
  {
      /* create in-rif */
      /* TTL/hop-limit threshold in valid only only in the egress, so for in-rif it is set to max - 255 */
	  rc = bcm_vlan_create(unit, in_vlan);
	  if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
		  printf("failed to create vlan %d", in_vlan);
	  }

	  rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
	  if (rc != BCM_E_NONE) {
		  printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
		  return rc;
	  }

      intf.vsi = in_vlan;
      intf.my_global_mac = mac_l3_ingress;
      intf.my_lsb_mac = mac_l3_ingress;
	  intf.vrf_valid = 1;
      intf.vrf = vrf;
	  intf.ttl_valid = 1;
      intf.ttl = 255;
      intf.qos_map_valid = 1;
      intf.qos_map_id = qos_map_id_l3_ingress_get(unit);
 
      rc = l3__intf_rif__create(unit, &intf);
      ingress_intf[0] = intf.rif;
      if (rc != BCM_E_NONE) {
		  printf("Error, l3__intf_rif__create");
      }

      print_mac_addr(mac_l3_ingress);
      printf ("create_l3_intf OK, in_port %d, in_vlan %d, vrf %d, ingress_intf[0] %d \n", in_port, in_vlan, vrf, ingress_intf[0]);

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

	  intf.vsi = out_vlan;
      intf.ttl = ttl_threshold;

	  rc = l3__intf_rif__create(unit, &intf);
      ingress_intf[1] = intf.rif;
      if (rc != BCM_E_NONE) {
		  printf("Error, l3__intf_rif__create");
      }

      print_mac_addr(mac_l3_ingress);
      printf ("create_l3_intf OK, out_port %d, out_vlan %d, vrf %d, ingress_intf[1] %d \n", out_port, out_vlan, vrf, ingress_intf[1]);

      /* create fec for UC IPV6 entry */
    create_l3_egress_s l3_eg;

    sal_memcpy(l3_eg.next_hop_mac_addr, mac_l3_egress, 6);  

    l3_eg.out_tunnel_or_rif   = ingress_intf[1];
    l3_eg.vlan   = out_vlan;
    l3_eg.out_gport  = out_port;
    l3_eg.l3_flags = CINT_NO_FLAGS;
    l3_eg.allocation_flags = CINT_NO_FLAGS;
    l3_eg.arp_encap_id = encap_id[0];
    l3_eg.fec_id = egress_intf[0];


    rc = l3__egress__create(unit, &l3_eg);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
        return rc;
    }

    encap_id[0] = l3_eg.arp_encap_id;
    egress_intf[0] = l3_eg.fec_id;

      print_mac_addr(mac_l3_egress);
      printf ("create_l3_egress OK, out_port %d, out_vlan %d, out_inf %d, out_fec %d \n", out_port, out_vlan, ingress_intf[1], egress_intf[0]);


      /* No MC */
      /* create MC group for MC IPV6 entry:
       * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
      /* mc_id[0] = create_ip_mc_group(unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[0], out_port, nof_mc_members, out_vlan);
      printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[0], ipmc_index[0], nof_mc_members, out_vlan); */

      /* Add vlan member to each outgoing port */
      /* for(port_ndx = 0; port_ndx < nof_mc_members; ++port_ndx) {        
        rc = bcm_vlan_gport_add(unit, out_vlan, out_port+port_ndx, 0);
        if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_port+port_ndx, out_vlan);
          return rc;
        }
      } */

      /* add also the in_port (with the in_vlan) to the MC group */
      /* rc = bcm_multicast_l3_encap_get(unit, mc_id[0], in_port, in_vlan, &cud);
      if (rc != BCM_E_NONE) {
            printf("Error, in bcm_petra_multicast_l3_encap_get, mc_group mc_group \n");
            return rc;
      }
      rc = bcm_multicast_ingress_add(unit, mc_id[0], in_port, cud);
      if (rc != BCM_E_NONE) {
         printf("Error, in bcm_multicast_egress_add, ipmc_index %d dest_gport $in_port \n", mc_id[0]);
         return rc;
      }*/
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

      intf.vsi = in_vlan;
      intf.my_global_mac = mac_l3_ingress;
      intf.my_lsb_mac = mac_l3_ingress;
	  intf.vrf_valid = 1;
      intf.vrf = vrf;
	  intf.ttl_valid = 1;
      intf.ttl = 255;
      intf.qos_map_valid = 1;
      intf.qos_map_id = qos_map_id_l3_ingress_get(unit);
 
      rc = l3__intf_rif__create(unit, &intf);
      ingress_intf[0] = intf.rif;
      if (rc != BCM_E_NONE) {
		  printf("Error, l3__intf_rif__create");
      }

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

      intf.vsi = out_vlan;
      intf.ttl = ttl_threshold;
 
      rc = l3__intf_rif__create(second_unit, &intf);
      second_ingress_intf[0] = intf.rif;
      if (rc != BCM_E_NONE) {
		  printf("Error, l3__intf_rif__create");
      }

      /* create fec for UC IPV6 entry */
    create_l3_egress_s l3_eg;

    l3_eg.out_tunnel_or_rif   = second_ingress_intf[0];
    l3_eg.vlan   = out_vlan;
    l3_eg.out_gport  = egress_port_list[0];
    l3_eg.l3_flags = CINT_NO_FLAGS;
    l3_eg.allocation_flags = CINT_NO_FLAGS;
    l3_eg.arp_encap_id = encap_id[0];
    l3_eg.fec_id = egress_intf[0];


    rc = l3__egress__create(second_unit, &l3_eg);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", egress_port_list[0]);
        return rc;
    }

    encap_id[0] = l3_eg.arp_encap_id;
    egress_intf[0] = l3_eg.fec_id;

      /* create MC group for MC IPV6 entry:
       * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
      /* mc_id[0] = create_ip_mc_group(second_unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[0], egress_port_list[0], nof_mc_members, out_vlan);
      printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[0], ipmc_index[0], nof_mc_members, out_vlan); 
      */
       
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

      intf.vsi = in_vlan;
      intf.ttl = 255;
 
      rc = l3__intf_rif__create(second_unit, &intf);
      ingress_intf[1] = intf.rif;
      if (rc != BCM_E_NONE) {
		  printf("Error, l3__intf_rif__create");
      }

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

      intf.vsi = out_vlan;
      intf.ttl = ttl_threshold;
 
      rc = l3__intf_rif__create(unit, &intf);
      second_ingress_intf[1] = intf.rif;
      if (rc != BCM_E_NONE) {
		  printf("Error, l3__intf_rif__create");
      }

      /* create fec for UC IPV6 entry */
      
    create_l3_egress_s l3_eg1;  

    l3_eg1.out_tunnel_or_rif   = second_ingress_intf[1];
    l3_eg1.vlan   = out_vlan;
    l3_eg1.out_gport  = egress_port_list[1];
    l3_eg1.l3_flags = CINT_NO_FLAGS;
    l3_eg1.allocation_flags = CINT_NO_FLAGS;
    l3_eg1.arp_encap_id = encap_id[1];
    l3_eg1.fec_id = egress_intf[1];


    rc = l3__egress__create(second_unit, &l3_eg1);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", egress_port_list[0]);
        return rc;
    }

    encap_id[1] = l3_eg1.arp_encap_id;
    egress_intf[1] = l3_eg1.fec_id;

      /* create MC group for MC IPV6 entry:
       * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
      /* mc_id[1] = create_ip_mc_group(unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[1], egress_port_list[1], nof_mc_members, out_vlan);
      printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[1], ipmc_index[1], nof_mc_members, out_vlan); 
      */ 
  }
 
  bcm_ip6_t route0;
  bcm_ip6_t route1;
  bcm_ip6_t route2;
  bcm_ip6_t route3;
  bcm_ip6_t route4;
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
  mask[15]= 0xff;
  mask[14]= 0xff;
  mask[13]= 0xff;
  mask[12]= 0xff;
  mask[11]= 0xff;
  mask[10]= 0xff;
  mask[9] = 0xff;
  mask[8] = 0xff;
  mask[7] = 0;
  mask[6] = 0;
  mask[5] = 0;
  mask[4] = 0;
  mask[3] = 0;
  mask[2] = 0;
  mask[1] = 0;
  mask[0] = 0;

  int i;
  for (i=0; i<4; i++) { 

    
    /* change the prefix length, for IPV6 UC */
      if (i == 1) {
          mask[8] = 0xfc;
      }
      else if (i == 2) {
          mask[8] = 0xf0;
      }
      else if (i == 3) {
          mask[8] = 0xc0;
      }

    /* add UC entries */
    if (second_unit<0) {
        if (i == 0) { 
            rc = add_route_ip6(unit, route0, mask, vrf, egress_intf[0]);
        }
        else if (i == 1) {
            rc = add_route_ip6(unit, route1, mask, vrf, egress_intf[0]);
        }
        else if (i == 2) {
            rc = add_route_ip6(unit, route2, mask, vrf, egress_intf[0]);
        }
        else { /*i == 3*/
            rc = add_route_ip6(unit, route3, mask, vrf, egress_intf[0]);
        }
    }
    else /* second_unit>=0 */
    {
        if (i == 0) 
            rc = add_route_ip6(unit, route0, mask, vrf, egress_intf[i%MAX_PORTS]);
        else if (i == 1) 
            rc = add_route_ip6(unit, route1, mask, vrf, egress_intf[i%MAX_PORTS]);
        else if (i == 2) 
            rc = add_route_ip6(second_unit, route2, mask, vrf, egress_intf[i%MAX_PORTS]);
        else /*i == 3*/
            rc = add_route_ip6(second_unit, route3, mask, vrf, egress_intf[i%MAX_PORTS]);
    }

    if (rc != BCM_E_NONE) {
        printf("Error. add_route_ip6 failed: %d\n", rc);
        return BCM_E_FAIL;
    }    

    if (i == 0) 
	    print_ipv6_addr("route", route0);
    else if (i == 1) 
	    print_ipv6_addr("route", route1);
    else if (i == 2) 
	    print_ipv6_addr("route", route2);
    else if (i == 3)
	    print_ipv6_addr("route", route3);

    if (i != 4) {
        printf("                                                ");
        print_ipv6_addr("mask", mask);
    }
  }

  if (ttl_threshold != 255) {

      /* set egress Action-Profile-TTL-Scope trap to drop all packets MC with TTL/hop-limit that is lower than the threshold */
      rc = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgIpmcTtlErr, &trap_id);
      if (rc != BCM_E_NONE) {
          printf("Error. bcm_rx_trap_type_create failed: $rc\n", rc);
          return BCM_E_FAIL;
      }

      sal_memset(&trap_config, 0, sizeof(trap_config));
      trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
      trap_config.dest_port = BCM_GPORT_BLACK_HOLE; /* drop */

      rc = bcm_rx_trap_set(unit, trap_id, &trap_config);
      if (rc != BCM_E_NONE) {
          printf("Error. bcm_rx_trap_set failed: $rc\n", rc);
          return BCM_E_FAIL;
      }
  }

  return 0;
}

/********** 
  IPv6 with RPF Tests
  Strict and Loose
 */

/********** 
  Test Global Vars
 */
bcm_mac_t mac_l3_ingress_2 = {0x00, 0x00, 0x00, 0x00, 0x55, 0x44}; /* incoming DA (myMAc) for second inRIF*/
int sip_in_rif_2_intf = 0;

/* group - field processor group correspond to the group that created in field_extended_ip_mpls_group_create() */
bcm_field_group_t group;


/********** 
  Test supporting functions
 */

/* creating the group for MPLS IPv4 DB,
   in this case the group is not configured in the HW but only created in the SW.
   (the group is created by init only when custom_feature_ext_ipv4_mpls_extended_fwd_table_size SOC property is set) */
int ipv6_rpf_field_extended_ip_mpls_group_create(/* in */ int unit,
                                        /* out */ bcm_field_group_t *group)

{

    int a;
    int rc;
/*  rc = field_acl_4_label_push_groups_set(unit, 1, 2, 3, 4, 5, 6, 7, &a);*/
/*  if (rc != BCM_E_NONE) {                                            */
/*      printf ("field_acl_4_label_push_groups_set failed: %d \n", rc);*/
/*      return rc;                                                     */
/*  }                                                                  */

    rc = field_extended_ipv6_group_create(unit, group);
    if (rc != BCM_E_NONE) {
        printf ("field_acl_4_label_push_groups_set failed: %d \n", rc);
    }
    return rc;
}

int 
field_ipv6_entry_add (int unit,bcm_field_group_t grp, bcm_ip6_t addr, bcm_ip6_t mask, uint32 in_vrf, int intf)

{
    int rc;

    /* Build forwarding decision only entry
     *  PPP state = 0, protection_pointer = 1023; &&
     *  eei_2_type = backup, eei_2_type = 0;
     */
    /* For IP Regular Forwarding must be set to 1023 */
    uint32 protection_pointer = 1023;
    /* refer as backup eei-2 */
    uint8 eei_2_type = 0;

    /* 
     * Below if how to encode the fwd bit array above                                                                                        |
    | 4b'0 | ctl = 3b'0| v = 1b'0 | outlif = 16b'0 | Dest FEC = 4b'0100 | fec id = 15b |
    */
    uint8 fwd[6] = {0x00,0x00,0x00,0x02,0x04,0x00};
    uint32 eei_2 = 0; /* Not important */
    uint32 fec = intf;
    int entry = -1;

    fwd[5] =  (fec & 0xFF);
    fwd[4] =  ((fec >> 8) & 0xFF );
/* Leave for Debug
    printf("using fwd = [");       */
/*  int i;                         */
/*  for (i = 0; i < 6; i++) {      */
/*      if (i < 5) {               */
/*          printf("0x%x,",fwd[i]);*/
/*      }                          */
/*      else {                     */
/*          printf("0x%x",fwd[i]); */
/*      }                          */
/*  }                              */
/*  printf("]\n");                 */

    /* Restore fec value */
    fec = intf;
    rc = field_extended_ipv6_entry_add(unit, grp, protection_pointer, eei_2_type, eei_2, fec, fwd,
                                     in_vrf, addr, mask, &entry);
    if (rc != BCM_E_NONE) {
      printf ("field_extended_ipv6_entry_add failed: %d \n", rc);
    }
    else {
        printf ("add_route_ip6 field_extended_ipv6_entry_add OK: %d \n", rc);
    }
    printf("field_extended_ipv6_entry index #%x", entry);

    return rc;
}


int
l3_ip_uc_rpf_config_traps(int unit, int rcy_port) 
{
 	int rv = BCM_E_NONE, i;
	bcm_rx_trap_config_t trap_config;
	int flags = 0;
    int L3_rpf_uc_strict_trap_id=0;
    int L3_rpf_uc_loose_trap_id=0;

    bcm_rx_trap_config_t trap_config_recycle;
    bcm_gport_t soc_routing_rcy_port;
    bcm_gport_t g_routing_rcy_port;
    bcm_gport_t recycle_port_iter;
    int trap_code;
    int SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS = 259;
    int use_hw_id = 0;   /* if changed to 1, then the trap HW id is used instead of the SW id.*/

    /* 
     * Configure the RPF trap to the recycle for second pass
     */

    soc_routing_rcy_port = rcy_port;
    BCM_GPORT_LOCAL_SET(g_routing_rcy_port, soc_routing_rcy_port);
    if (g_routing_rcy_port == -1) {
        printf("Error, in trap create, get rcy trap gport\n");
        rv = BCM_E_PORT;
        return rv;
    }


    bcm_rx_trap_config_t_init(&trap_config_recycle);
    trap_config_recycle.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_BYPASS_FILTERS);
    trap_config_recycle.flags |= (BCM_RX_TRAP_LEARN_DISABLE | BCM_RX_TRAP_WITH_ID);
    trap_config_recycle.dest_port = g_routing_rcy_port;
    trap_config_recycle.trap_strength = 0;

    /*
     * Capture the SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS trap 
     */

    if (use_hw_id)
    {
        trap_code  = 233;
    }
    else
    {
        trap_code  = SOC_PPC_TRAP_CODE_IPV6_UC_RPF_2PASS;
    }

    rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) 
    {
        /* Trap is already captured */
        printf("IPv6 UC with RPF 2pass trap:0%x already captured, This is OK\n",trap_code);
    }
    /* 
     * Trap setting for IPv6 UC with PRF 2 pass.
     */
    rv = bcm_rx_trap_set(unit, trap_code, &trap_config_recycle);
    if (rv != BCM_E_NONE) 
    {
        /* Trap is set failed */
        printf("IPv6 UC with RPF 2pass trap to recycle port setting failed\n");
    }


    /* 
     * RPF Error Trap types Set
     */
    if(L3_rpf_uc_strict_trap_id == 0){
        /*rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUcStrictRpfFail, &L3_rpf_uc_strict_trap_id); */
        rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcStrictRpfFail, &L3_rpf_uc_strict_trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in trap create, trap bcmRxTrapUcStrictRpfFail \n");
            return rv;
        }
    }

    /* set uc loose to drop packet */
    if(L3_rpf_uc_loose_trap_id == 0){
        /*rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapUcLooseRpfFail,&L3_rpf_uc_loose_trap_id);*/
        rv = bcm_rx_trap_type_create(unit,0,bcmRxTrapUcLooseRpfFail,&L3_rpf_uc_loose_trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in trap create, trap bcmRxTrapUcLooseRpfFail \n");
            return rv;
        }
    }
    /*change dest port for trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags |= (BCM_RX_TRAP_UPDATE_DEST); 
    trap_config.trap_strength = 7;
    trap_config.dest_port=BCM_GPORT_BLACK_HOLE; 
    rv = bcm_rx_trap_set(unit,L3_rpf_uc_loose_trap_id,&trap_config);
    if (rv != BCM_E_NONE) {
      printf("Error, in trap set L3_rpf_uc_loose_trap_id \n");
      return rv;
    }

    rv = bcm_rx_trap_set(unit,L3_rpf_uc_strict_trap_id,&trap_config);
    if (rv != BCM_E_NONE) {
      printf("Error, in trap set L3_rpf_uc_strict\n");
      return rv;
    }
    print L3_rpf_uc_strict_trap_id;
    print L3_rpf_uc_loose_trap_id;

    return rv;
}

int
create_l3_ipv6_uc_inf(int unit, int in_port, int out_port)
{
    /* Call bcm_l3_ip6 to set the standard setting
     * vrf = 3;
     * in_vlan = 17;  == inRif
     * out_vlan = 18; == outRif
     * mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};  [egress DA]
     */
    int rc;
       
    rc = bcm_l3_ip6(unit, -1, in_port, out_port);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_ip6_uc_with_rpf: create_l3_intf failed: %d \n", rc);
        return rc;
    }
    return rc;
}

int
update_l3_rpf_mode(int unit)
{
    int rc;

    /* Define RIF uRPF mode both to strict */
    bcm_l3_ingress_urpf_mode_t rpf_modes[] = {
        bcmL3IngressUrpfStrict,   /* Strickt mode Unicast RPF. */
        bcmL3IngressUrpfLoose,    /* Loose mode Unicast RPF. */
        bcmL3IngressUrpfDisable   /* No RPF. */
    };
    /* 
     *  Update for the InRifA (myMAC==mac_l3_ingress) the uRFP to Loose
     */
    printf ("update_l3_rpf_mode: ingress_intf: %d \n", ingress_intf[0]);
    rc = modify_l3_interface_ingress_rpf(unit, bcmL3IngressUrpfLoose, ingress_intf[0]);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_ip6_uc_with_rpf: modify_l3_interface_ingress_rpf failed: %d for inRif %d\n", rc, ingress_intf[0]);
        return rc;
    }
    else {
        printf ("bcm_l3_ip6_uc_with_rpf: modify_l3_interface_ingress_rpf set to Loose, for inRif %d\n", ingress_intf[0]);
    }

    /* 
     *  Update for the InRifA (myMAC==mac_l3_ingress_2) the uRFP to Strict
     */
    printf ("update_l3_rpf_mode: ingress_intf: %d \n", sip_in_rif_2_intf);
    rc = modify_l3_interface_ingress_rpf(unit, bcmL3IngressUrpfStrict, sip_in_rif_2_intf);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_ip6_uc_with_rpf: modify_l3_interface_ingress_rpf failed: %d for inRif %d\n", rc, sip_in_rif_2_intf);
        return rc;
    }
    printf ("bcm_l3_ip6_uc_with_rpf: modify_l3_interface_ingress_rpf set to Strict, for inRif %d\n", sip_in_rif_2_intf);

    return BCM_E_NONE;
}


int
vlan_port_create_vsi( int unit, bcm_port_t port_id, 
    bcm_gport_t *gport, 
    bcm_vlan_t vlan, /* incoming outer vlan and also outgoing vlan */
    int vsi)
{

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);
  
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port_id;
    vp.match_vlan = vlan; 
    vp.egress_vlan = vlan; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.vsi = vsi; 
    vp.flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE; 
    /*vp.flags = 0;*/
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;  
    return BCM_E_NONE;
}


/*
 * 
 packet will be routed from in_port to out-port 
 * packet processing doesn't use FEC for outRIF, 
 * yet the next-hop mac is pointed directly from host lookup
 * host lookup returns: <dest-port, FEC, MAC (pointed by outlif)
 * FEC returns out-RIF.
  * HOST: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 2.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - IPV6 DIP = 01001600350070000000db0712000000 (0100:1600:7800:8800:0000:db07:1200:0000)
 * expected: 
 *  - out port = out_port
 *  - vlan 100.
 *  - DA = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 */
int bcm_l3_ip6_uc_with_rpf_2pass(int unit, int second_unit ,int in_port, int out_port) {

    int rc;
    int port_ndx;

    int L3_urpf_mode_idx;

    bcm_mac_t sa_mac_l3_ingress  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; /* incoming SA */
    int sip_fec_id = 0;
    int sip_encap_id = 0;
    int sip_fec_id_2 = 0;
    int sip_encap_id_2 = 0;
    int in_vlan = 17;
    int out_vlan = 18;
    int in_vlan_2 = in_vlan + 10; /* Set another vlan id for false SIP inRIF (inRIF == vid) */
    int CINT_NO_FLAGS = 0;

    bcm_ip6_t route5, route6; /* SIP data */
    bcm_ip6_t mask;
    int vrf = 3;

    bcm_gport_t lif;
    

    /* Make sure the device is Arad+ as uRPF per RIF is supported only on Arad+ and above. */
    if (!is_device_or_above(unit,ARAD_PLUS)) {
        printf("Error - IPv6 uRPF mode per RIF is only supported for Arad+.\n");
        return BCM_E_FAIL;
    }

    rc = ipv6_rpf_field_extended_ip_mpls_group_create (unit, &group);
    if (rc != BCM_E_NONE) {
        printf("Error create Field gorup for KBP / FLP, error %x.\n", rc);
        return rc;
    }
	printf("Field gorup for KBP / FLP created - %d.\n", group);

    /* Set recycle port PTC
     *  For the test aligned with dvapi we always use port #40
     */
    int rcy_port = 40;
    rc = bcm_port_control_set(unit, rcy_port, bcmPortControlOverlayRecycle, 1);
    if (rc != BCM_E_NONE) {
        printf("Error setting Recycle Port %d PTC to bcmPortControlOverlayRecycle, error %x.\n",rcy_port, rc);
        return rc;
    }

    rc = l3_ip_uc_rpf_config_traps(unit, 40);
    if (rc != BCM_E_NONE) {
      return rc;
    } 

    /* Create a LIF for each RIF. This is important for the uRPF mode to work per RIF. */
    /* Create a lif for <vlan,port>. */
    rc = vlan_port_create_vsi(unit, in_port, &lif, in_vlan, in_vlan);
    if (rc != BCM_E_NONE) {
        printf("Error, in vlan_port_create\n");
        return rc;
    }
    printf ("lif %d vlan %d\n", lif, in_vlan);
    rc = vlan_port_create_vsi(unit, in_port, &lif, in_vlan_2, in_vlan_2);
    if (rc != BCM_E_NONE) {
        printf("Error, in vlan_port_create\n");
        return rc;
    }
    printf ("lif %d vlan %d\n", lif, in_vlan_2);

    /* Create system SIP & DIP inLIF & inRIF */
    rc = create_l3_ipv6_uc_inf(unit, in_port, out_port); /* rpf_modes[L3_urpf_mode_idx]); */
    if (rc != BCM_E_NONE) {
      return rc;
    }

    /* Add FEC for UC IPV6 Ingress_MAC + in_vlan(17)*/
    create_l3_egress_s l3_eg;

    sal_memcpy(l3_eg.next_hop_mac_addr, mac_l3_ingress, 6);  

    l3_eg.out_tunnel_or_rif   = ingress_intf[0];
    l3_eg.vlan   = in_vlan;
    l3_eg.out_gport  = in_port;
    l3_eg.l3_flags = CINT_NO_FLAGS;
    l3_eg.allocation_flags = CINT_NO_FLAGS;
    l3_eg.arp_encap_id = sip_encap_id;
    l3_eg.fec_id = sip_fec_id;


    rc = l3__egress__create(unit, &l3_eg);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
        return rc;
    }

    sip_encap_id = l3_eg.arp_encap_id;
    sip_fec_id = l3_eg.fec_id;

    print_mac_addr(mac_l3_ingress);
    printf ("create_l3_egress OK, in_port %d, in_vlan %d, in_inf %d, sip_fec %d \n", in_port, in_vlan, ingress_intf[0], sip_fec_id);


    /* create SIP-2 inRif-2 */
	rc = bcm_vlan_create(unit, in_vlan_2);
	if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
	  printf("failed to create vlan %d", in_vlan_2);
	}

	rc = bcm_vlan_gport_add(unit, in_vlan_2, in_port, 0);
	if (rc != BCM_E_NONE) {
	  printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan_2);
	  return rc;
	}

    create_l3_intf_s intf;
    intf.vsi = in_vlan_2;
    intf.my_global_mac = mac_l3_ingress_2;
    intf.my_lsb_mac = mac_l3_ingress_2;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    intf.ttl_valid = 1;
    intf.ttl = 255;
    intf.qos_map_valid = 1;
    intf.qos_map_id = qos_map_id_l3_ingress_get(unit);
    
    rc = l3__intf_rif__create(unit, &intf);
    sip_in_rif_2_intf = intf.rif;
    if (rc != BCM_E_NONE) {
      printf("Error, l3__intf_rif__create");
    }
    
    print_mac_addr(mac_l3_ingress_2);
    printf ("create_l3_intf OK, in_port %d, in_vlan %d, vrf %d, sip_in_rif_2_intf %d\n", in_port, in_vlan_2, vrf, sip_in_rif_2_intf);
    
    create_l3_egress_s l3_eg;

    sal_memcpy(l3_eg.next_hop_mac_addr, sa_mac_l3_ingress, 6);  

    l3_eg.out_tunnel_or_rif   = sip_in_rif_2_intf;
    l3_eg.vlan   = in_vlan_2;
    l3_eg.out_gport  = in_port;
    l3_eg.l3_flags = CINT_NO_FLAGS;
    l3_eg.allocation_flags = CINT_NO_FLAGS;
    l3_eg.arp_encap_id = sip_encap_id_2;
    l3_eg.fec_id = sip_fec_id_2;


    rc = l3__egress__create(unit, &l3_eg);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
        return rc;
    }

    sip_encap_id_2 = l3_eg.arp_encap_id;
    sip_fec_id_2 = l3_eg.fec_id;

    print_mac_addr(sa_mac_l3_ingress);
    printf ("create_l3_egress OK, in_port %d, in_vlan_2 %d, sip_in_rif_2_intf %d, sip_fec_id_2 %d\n", in_port, in_vlan_2, sip_in_rif_2_intf, sip_fec_id_2);
    
    rc = update_l3_rpf_mode(unit);
    if (rc != BCM_E_NONE) {
        printf ("update_l3_rpf_mode failed: %d \n", rc);
    return rc;
    }
    
    /* create FEC for SIP UC IPV6 entry */
    /* Note - In-RIF was created above for the in_port/in_vlan_2/vrf */
    /* printf ("ingress_intf[0] - PACKET classifcation RIF %x , Out_Rif ingress_intf[1] %x, FEC classification RIF %x \n", ingress_intf[0], ingress_intf[1], sip_in_rif_2_intf); */
    
    
    /* Add SIP Routing Entry / IN-RIF / FEC and OUT-RIF  */    
    /* SIP ==    0000:0000:0000:0000:0000:0000:0000:0123 */
    /* UC IPV6 SIP: */
    route5[15]= 0x23; /* LSB */
    route5[14]= 0x01;
    route5[13]= 0x00;
    route5[12]= 0x00;
    route5[11]= 0x00;
    route5[10]= 0x00;
    route5[9] = 0x00;
    route5[8] = 0x00;
    route5[7] = 0x00;
    route5[6] = 0x00;
    route5[5] = 0x00;
    route5[4] = 0x00;
    route5[3] = 0x00;
    route5[2] = 0x01;
    route5[1] = 0x00;
    route5[0] = 0x01; /* MSB */
    
    /* UC IPV6 DIP: */
    route6[15]= 0x35; /* LSB */
    route6[14]= 0x01;
    route6[13]= 0x00;
    route6[12]= 0x00;
    route6[11]= 0x00;
    route6[10]= 0x00;
    route6[9] = 0x00;
    route6[8] = 0x00;
    route6[7] = 0x00;
    route6[6] = 0x00;
    route6[5] = 0x00;
    route6[4] = 0x00;
    route6[3] = 0x00;
    route6[2] = 0x02;
    route6[1] = 0x00;
    route6[0] = 0x02; /* MSB */
    
    /* UC IPV6 SIP MASK: */
    mask[15]= 0xff;
    mask[14]= 0xff;
    mask[13]= 0xff;
    mask[12]= 0xff;
    mask[11]= 0xff;
    mask[10]= 0xff;
    mask[9] = 0xff;
    mask[8] = 0xff;
    mask[7] = 0x0;
    mask[6] = 0x0;
    mask[5] = 0x0;
    mask[4] = 0x0;
    mask[3] = 0x0;
    mask[2] = 0x0;
    mask[1] = 0x0;
    mask[0] = 0x0;
    
    /* Use FEC and create a new route entry for the SIP*/
    rc = add_route_ip6(unit, route5, mask, vrf, sip_fec_id);
    if (rc != BCM_E_NONE) {
        printf("Error. SIP bcm_l3_ip6_uc_with_rpf: add_route_ip6 failed: %d\n", rc);
        return BCM_E_FAIL;
    } 
    
    print_ipv6_addr("route", route5);
    print_ipv6_addr("mask", mask);
    
    /* Use FEC and create a new route entry for the SIP*/
    rc = add_route_ip6(unit, route6, mask, vrf, sip_fec_id_2);
    if (rc != BCM_E_NONE) {
        printf("Error. SIP bcm_l3_ip6_uc_with_rpf: add_route_ip6 failed: %d\n", rc);
        return BCM_E_FAIL;
    } 
    
    print_ipv6_addr("route", route6);
    print_ipv6_addr("mask", mask);
    
    return rc; /* Success */
}



