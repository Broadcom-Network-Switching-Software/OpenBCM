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
cint ../sand/utility/cint_sand_utils_mpls.c
cint utility/cint_utils_l3.c 
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c 
cint 
print qos_map_l3(unit); 
print bcm_l3_ip6(unit, <second_unit>, <in_port>, <out_port>);
 
for one device: call bcm_l3_ip6() with second_unit<0
 
for two devices: make sure that master device is unit 0 with modid 0 and slave device is unit 1 with modid 1

traffic example for two devices: 
 
    run ethernet header with DA 0:0:0:0:55:34 (for UC) or 33:33:3:2:1:0 (for compatible-MC) and vlan tag id 17
    and IPV6 header with DIP according to the following:
    
    device 0:
    ---------
      <in_port>:    route  01001600350070000000db0700000000 : 100:1600:3500:7000:0:db07:0:0  
                    mask   0000000000000000ffffffffffffffff : 0:0:0:0:ffff:ffff:ffff:ffff
 
                    route  01001600350064000000db0700000000 : 100:1600:3500:6400:0:db07:0:0 
                    mask   0000000000000000fcffffffffffffff : 0:0:0:0:fcff:ffff:ffff:ffff
                    
                    route  ff1e0d0c0b0a09080706050403020100 : ff1e:d0c:b0a:908:706:504:302:100 (comaptible-MC)   
                    
    device 1:
    ---------
      <in_port>:    route  01001600550078000000db0700000000 : 100:1600:5500:7800:0:db07:0:0 
                    mask   0000000000000000f0ffffffffffffff : 0:0:0:0:f0ff:ffff:ffff:ffff
                    
                    route  01001600780088000000db0700000000 : 100:1600:7800:8800:0:db07:0:0 
                    mask   0000000000000000c0ffffffffffffff : 0:0:0:0:c0ff:ffff:ffff:ffff
                    
                    route  ff1e0d0c0b0a09080706050403020100 : ff1e:d0c:b0a:908:706:504:302:100 (comaptible-MC) 
                  
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
 
to test IPV6 default route: 
--------------------------- 
call: 
ipv6_default_dest_set(unit, <mc>, 1, 14); 
 
and run unknown UC/MC traffic (depends on whether <mc> is set or not). 
this will set the default destination to be a trapm pointing at <out_port>, so the unknown traffic will go there. 
 
now call: 
ipv6_default_dest_set(unit, <mc>, 0, 14); 
 
nad run again the same MC/UC IPV6 traffic. 
this will set the default route to be VRF and all traffic will be dropped. 
 
-------------------------------------------------------------------------------- 
 
The cint can be also used to test RIF TTL thershold:
In this example, same out_rif (=18) is created, but this time its TTL threshold is set as well. 
If an IPV6 MC packet arrives with a hop-limit that is lower than the threshold, the packet will be dropped at egress.  
calling sequence: 
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c 
cint utility/cint_utils_l3.c 
cint 
print qos_map_l3(unit); 
print bcm_l3_ip6_with_ttl(unit, -1, <in_port>, <out_port>, <ttl_threshold>); 
 
 
The cint can be also used to test mtu by calling modify_l3_interface_ingress(int unit, int mtu, int out_vlan)
after bcm_l3_ip6. In this example out_vlan is set to 18, so to disable mtu use the following sequence:
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c
cint utility/cint_utils_l3.c  
cint 
print qos_map_l3(unit); 
print bcm_l3_ip6(unit,-1,13,14);
print modify_l3_interface_ingress(unit, 0, 18);
where out-vlan 18 is the outgoing interface 
 
 
 
  Note: this cint also includes tests for 4 label push and protection 

*/

bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34}; /* incoming DA (myMAc) */

/* Globals */
int _g_bcmRxTrapDefaultMcv6_trap_id=-1;
int _g_bcmRxTrapDefaultUcv6_trap_id=-1;
int default_vrf=0;
int ipv6_fap_default_intf=0;

/* enable to set / unset UC routes
   Used in case required to run ipv6 MC tests only
   for MC routsb only use:
         bcm_l3_ip6_mc(int unit, int second_unit ,int in_port, int out_port)
   for MC & UC routes use:
         bcm_l3_ip6(int unit, int second_unit ,int in_port, int out_port)
*/
int add_ipv6_uc_routes=1;


int preserving_dscp=0; /*Preserving DSCP on a per out-port bases*/

/*
 * modify l3 interface ingress for mtu check
 *  Parmeters:
 *  - mtu - maximal transmission unit
 *  - out_vlan - out vlan id
 */
int modify_l3_interface_ingress(int unit, int mtu, int out_vlan) {

  int CINT_NO_FLAGS = 0;
  int rc;
  bcm_l3_intf_t l3if, l3if_ori;

  bcm_l3_intf_t_init(&l3if);

  /* before creating L3 INTF, check whether L3 INTF already exists*/
  bcm_l3_intf_t_init(&l3if_ori);
  l3if_ori.l3a_intf_id = out_vlan;
  rc = bcm_l3_intf_get(unit, &l3if_ori);
  if (rc == BCM_E_NONE) {
      /* if L3 INTF already exists, replace it*/
      l3if.l3a_flags = CINT_NO_FLAGS | BCM_L3_REPLACE | BCM_L3_WITH_ID;
      l3if.l3a_intf_id = out_vlan;
  }
  else {
      l3if.l3a_flags = CINT_NO_FLAGS; 
  }

  sal_memcpy(l3if.l3a_mac_addr, mac_l3_ingress, 6);
  l3if.l3a_vid = out_vlan;
  l3if.l3a_ttl = 31;
  l3if.l3a_mtu = mtu;

  rc = bcm_l3_intf_create(unit, l3if);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_intf_create failed: %d \n", rc);
    }
  return rc;
}

/*
 * delete l3 interface
 */
int delete_l3_ingress(int unit) {

  int rc;

  rc = bcm_l3_intf_delete_all(unit);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_intf_delete_all failed: %d \n", rc);
    }

  return rc;
}



/************************************
 * Utilities
 */
void print_host_ipv6(char *type, bcm_ip6_t host, int vrf)
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


/*
 * Add Route
 */
int add_route_ip6(int unit, bcm_ip6_t *addr, bcm_ip6_t *mask, int vrf, int intf) {

  int rc;
  bcm_l3_route_t l3rt;

  bcm_l3_route_t_init(&l3rt);

  l3rt.l3a_flags = BCM_L3_IP6;
  if (lpm_raw_data_enabled) {
      l3rt.l3a_flags2 = BCM_L3_FLAGS2_RAW_ENTRY;
      l3rt.l3a_intf = lpm_payload;
      l3rt.l3a_vrf = 1;
  } else {
      l3rt.l3a_intf = intf;
      l3rt.l3a_vrf = vrf;
  }
  sal_memcpy(&(l3rt.l3a_ip6_net),(addr),16);
  sal_memcpy(&(l3rt.l3a_ip6_mask),(mask),16);
  l3rt.l3a_modid = 0;
  l3rt.l3a_port_tgid = 0;

  rc = bcm_l3_route_add(unit, l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %d \n", rc);
  }
  
  bcm_l3_route_t_init(&l3rt);
  l3rt.l3a_flags = BCM_L3_IP6;
  if (lpm_raw_data_enabled) {
      l3rt.l3a_flags2 = BCM_L3_FLAGS2_RAW_ENTRY;
      l3rt.l3a_vrf = 1;
  } else {
      l3rt.l3a_vrf = vrf; 
  }
  
  sal_memcpy(&(l3rt.l3a_ip6_net),(addr),16);
  sal_memcpy(&(l3rt.l3a_ip6_mask),(mask),16);
  rc = bcm_l3_route_get(unit, &l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_get failed: %d \n", rc);
  }
  print l3rt;
  
  return rc;
}

/*
 * Add IPV6 Host
 */
int add_ipv6_host(int unit, bcm_ip6_t *addr, int vrf, int fec, int intf) {
  int rc;

  if (is_device_or_above(unit, JERICHO)) {
      /* In Jericho and above IPv6 entries are stored in the KAPS, using bcm_l3_route_add */
      bcm_ip6_t l3a_ip6_mask   = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
      bcm_l3_route_t l3rt;

      bcm_l3_route_t_init(&l3rt);

      l3rt.l3a_flags = BCM_L3_IP6;
      l3rt.l3a_intf = fec;
      l3rt.l3a_vrf = vrf;
      sal_memcpy(&(l3rt.l3a_ip6_net),(addr),16);
      sal_memcpy(&(l3rt.l3a_ip6_mask),(l3a_ip6_mask),16);
      l3rt.l3a_modid = 0;
      l3rt.l3a_port_tgid = 0;

      rc = bcm_l3_route_add(unit, l3rt);
      if (rc != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %d \n", rc);
      }

      /* If the scale feature is enabled, also add the entries to the special KAPS tables */
      if ((soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_LONG, 0)) ||
          (soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_SHORT, 0)))
      {
          l3rt.l3a_flags2  |= BCM_L3_FLAGS2_SCALE_ROUTE;
          rc = bcm_l3_route_add(unit, l3rt);
          if (rc != BCM_E_NONE) {
            printf ("bcm_l3_route_add failed: %d \n", rc);
          }
      }
  } else {
      bcm_l3_host_t l3host;
      bcm_l3_host_t_init(l3host);

      l3host.l3a_flags =  BCM_L3_IP6;
      sal_memcpy(&(l3host.l3a_ip6_addr),(addr),16);
      l3host.l3a_vrf = vrf;
      /* dest is FEC + OutLif */
      l3host.l3a_intf = fec; /* fec */
      l3host.encap_id = intf;
      l3host.l3a_modid = 0;
      l3host.l3a_port_tgid = 0;

      rc = bcm_l3_host_add(unit, l3host);
      if (rc != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %d \n", rc);
      }
  }

  return rc;
}

/*
 * Set IPV6 default dest 
 *      mc: if set config default dest for IPV6 MC, else UC 
 *      trap: if set make default dest according to trap code, else VRF default
 *      out_port: used only if trap is set, as the trap dest
 */
int ipv6_default_dest_set(int unit, int mc, int trap, int out_port) {
  int rc;
  int trap_id;
  bcm_rx_trap_t trap_type;
  bcm_rx_trap_config_t trap_cfg;

  if (mc) {
      trap_type = bcmRxTrapDefaultMcv6;
	  if (_g_bcmRxTrapDefaultMcv6_trap_id >= 0) {
          /* Trap already set, using existing */
          trap_id = _g_bcmRxTrapDefaultMcv6_trap_id;
	  } else {
		  /* Trap no set yet */
		  rc = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_id);
		  _g_bcmRxTrapDefaultMcv6_trap_id = trap_id;
		  if (rc != BCM_E_NONE) {
			printf ("bcm_rx_trap_type_create failed: %d \n", rc);
		  }
	  }
  }
  else { /* UC */
      trap_type = bcmRxTrapDefaultUcv6;
      if (_g_bcmRxTrapDefaultUcv6_trap_id >= 0) {
          /* Trap already set, using existing */
          trap_id = _g_bcmRxTrapDefaultUcv6_trap_id;
	  } else {
		  /* Trap no set yet */
		  rc = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_id);
		  _g_bcmRxTrapDefaultUcv6_trap_id = trap_id;
		  if (rc != BCM_E_NONE) {
			printf ("bcm_rx_trap_type_create failed: %d \n", rc);
		  }
	  }
  }

  bcm_rx_trap_config_t_init(&trap_cfg);
  if (trap) {
      trap_cfg.trap_strength = 7;
  }
  else { /* VRF */
      trap_cfg.trap_strength = 0;
  }
  trap_cfg.flags |= BCM_RX_TRAP_UPDATE_DEST;
  trap_cfg.dest_port = out_port;

  rc = bcm_rx_trap_set(unit, trap_id, &trap_cfg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_rx_trap_set failed: %d \n", rc);
  }
  return rc;
}

/* delete functions */
int delete_host(int unit, int intf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_intf = intf;

  rc = bcm_l3_host_delete_all(unit, l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_delete_all failed: %d \n", rc);
  }
  return rc;
}


int MAX_PORTS=2;
/* use raw data payload in KAPs LPM */
int lpm_raw_data_enabled =0;
/* raw payload in Kaps*/
int lpm_payload = 0x12345;
int bcm_l3_ip6(int unit, int second_unit ,int in_port, int out_port, int preserv_dsp_port) {

    /*Preserving DSCP on a per out-port bases*/
    if (preserving_dscp) {
        int rv;
        rv = bcm_port_control_set(unit, preserv_dsp_port, bcmPortControlTCPriority, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, in function bcm_port_control_set, \n");
            return rv;
        }
    }

    /* redirect the packet in the pmf acording to the raw payload recived in the kaps*/
    if (lpm_raw_data_enabled) {
        int rv;
        rv = kaps_raw_data_redirect(unit); 
        if (rv != BCM_E_NONE) {
            printf("Error, in function kaps_raw_data_redirect, \n");
            return rv;
        }
    }

   return bcm_l3_ip6_with_ttl(unit, second_unit, in_port, out_port, 255);

}

int bcm_l3_ip6_mc_with_ttl(int unit, int second_unit, int in_port, int out_port, int ttl_threshold) {

    int rv;
    add_ipv6_uc_routes=0;

    rv = bcm_l3_ip6_with_ttl(unit, second_unit, in_port, out_port, ttl_threshold);
    /* restore add UC entries default */
    add_ipv6_uc_routes=1;
    return rv;
}

int bcm_l3_ip6_mc(int unit, int second_unit ,int in_port, int out_port) {

    int rv;
    add_ipv6_uc_routes=0;

    rv = bcm_l3_ip6_with_ttl(unit, second_unit, in_port, out_port, 255);
    /* restore add UC entries default */
    add_ipv6_uc_routes=1;
    return rv;
}

int bcm_l3_ip6_with_ttl(int unit, int second_unit, int in_port, int out_port, int ttl_threshold) {

  int CINT_NO_FLAGS = 0;
  int vrf = 3;
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

  bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */

  create_l3_intf_s intf;

  /*bshell(unit, "dbm bcm +l3 +api +verbose");*/
  
  if (lpm_raw_data_enabled) {
  	vrf=1;
  }

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

      /* create fec for UC IPV6 entry */
      create_l3_egress_s l3eg;

      l3eg.out_tunnel_or_rif = ingress_intf[1];
      sal_memcpy(l3eg.next_hop_mac_addr, mac_l3_egress, 6);  
      l3eg.vlan   = out_vlan;
      l3eg.out_gport   = out_port;
      l3eg.arp_encap_id = encap_id[0];
      l3eg.fec_id = egress_intf[0]; 
      l3eg.allocation_flags = CINT_NO_FLAGS;
      l3eg.l3_flags = CINT_NO_FLAGS;

      rc = l3__egress__create(unit,&l3eg);
      if (rc != BCM_E_NONE) {
          printf("Error, create egress object, out_port=%d, \n", out_port);
          return rc;
      }
	  encap_id[0] = l3eg.arp_encap_id;
	  egress_intf[0] = l3eg.fec_id;

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

		  intf.vsi = in_vlan + 2;
		  if (lpm_raw_data_enabled){
		      intf.vrf = 1;
	      } else {
		      intf.vrf = 0;
		  }
		  intf.ttl = 255;

		  rc = l3__intf_rif__create(unit, &intf);
		  ingress_intf[2] = intf.rif;
		  if (rc != BCM_E_NONE) {
			  printf("Error, l3__intf_rif__create");
		  }

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

		  intf.vsi = out_vlan + 2;
		  intf.ttl = ttl_threshold;

		  rc = l3__intf_rif__create(unit, &intf);
		  ingress_intf[3] = intf.rif;
		  if (rc != BCM_E_NONE) {
			  printf("Error, l3__intf_rif__create");
		  }

          /* create fec for UC IPV6 entry */
          create_l3_egress_s l3eg1;

          l3eg1.out_tunnel_or_rif = ingress_intf[3];
          sal_memcpy(l3eg1.next_hop_mac_addr, mac_l3_egress, 6);  
          l3eg1.vlan   = out_vlan + 2;
          l3eg1.out_gport   = out_port;
          l3eg1.arp_encap_id = encap_id[1];
          l3eg1.fec_id = egress_intf[1]; 
          l3eg1.allocation_flags = CINT_NO_FLAGS;
          l3eg1.l3_flags = CINT_NO_FLAGS;

          rc = l3__egress__create(unit,&l3eg1);
          if (rc != BCM_E_NONE) {
              printf("Error, create egress object, out_port=%d, \n", out_port);
              return rc;
          }
		  encap_id[1] = l3eg1.arp_encap_id ;
          egress_intf[1] = l3eg1.fec_id; 
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

      /* add also the in_port (with the in_vlan) to the MC group */
      rc = bcm_multicast_l3_encap_get(unit, mc_id[0], in_port, in_vlan, &cud);
      if (rc != BCM_E_NONE) {
            printf("Error, in bcm_petra_multicast_l3_encap_get, mc_group mc_group \n");
            return rc;
      }
      rc = bcm_multicast_ingress_add(unit, mc_id[0], in_port, cud);
      if (rc != BCM_E_NONE) {
         printf("Error, in bcm_multicast_egress_add, ipmc_index %d dest_gport $in_port \n", mc_id[0]);
         return rc;
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
      create_l3_egress_s l3eg;

      l3eg.out_tunnel_or_rif = second_ingress_intf[0];
      sal_memcpy(l3eg.next_hop_mac_addr, mac_l3_egress, 6);  
      l3eg.vlan   = out_vlan;
      l3eg.out_gport   = egress_port_list[0];
      l3eg.arp_encap_id = encap_id[0];
      l3eg.fec_id = egress_intf[0]; 
      l3eg.allocation_flags = CINT_NO_FLAGS;
      l3eg.l3_flags = CINT_NO_FLAGS;

      rc = l3__egress__create(second_unit,&l3eg);
      if (rc != BCM_E_NONE) {
          printf("Error, create egress object, out_port=%d, \n", out_port);
          return rc;
      }
	  
	  encap_id[0] = l3eg.arp_encap_id;
      egress_intf[0] = l3eg.fec_id;

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
      create_l3_egress_s l3eg1;

      l3eg1.out_tunnel_or_rif = second_ingress_intf[1];
      sal_memcpy(l3eg1.next_hop_mac_addr, mac_l3_egress, 6);  
      l3eg1.vlan   = out_vlan;
      l3eg1.out_gport   = egress_port_list[1];
      l3eg1.arp_encap_id = encap_id[1];
      l3eg1.fec_id = egress_intf[1]; 
      l3eg1.allocation_flags = CINT_NO_FLAGS;
      l3eg1.l3_flags = CINT_NO_FLAGS;

      rc = l3__egress__create(unit,&l3eg1);
      if (rc != BCM_E_NONE) {
          printf("Error, create egress object, out_port=%d, \n", out_port);
          return rc;
      }
	  encap_id[1] = l3eg1.arp_encap_id;
      egress_intf[1] = l3eg1.fec_id;

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
  for (i=0; i<6; i++) { 

    if (i == 4) {
        /* add MC entry */
        rc = find_ip6mc_entry(unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0]);
        if (rc != BCM_E_NONE)
        {
            rc = add_ip6mc_entry(unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0], vrf);            
            if (rc != BCM_E_NONE) {
                printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                return BCM_E_FAIL;
            }   
        }
        if (second_unit>=0)
        {
            rc = find_ip6mc_entry(second_unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1]);
            if (rc != BCM_E_NONE) {
                rc = add_ip6mc_entry(second_unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1]);
                if (rc != BCM_E_NONE) {
                    printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                    return BCM_E_FAIL;
                } 
            }
        }
    }
	else if (i == 5){
        /* add MC entry */
        rc = find_ip6mc_entry(unit, route5 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0]);
        if (rc != BCM_E_NONE)
        {
            rc = add_ip6mc_entry(unit, route5 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0], vrf);            
            if (rc != BCM_E_NONE) {
                printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                return BCM_E_FAIL;
            }   
        }
        if (second_unit>=0)
        {
            rc = find_ip6mc_entry(second_unit, route5 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1]);
            if (rc != BCM_E_NONE) {
                rc = add_ip6mc_entry(second_unit, route5 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1], vrf);
                if (rc != BCM_E_NONE) {
                    printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                    return BCM_E_FAIL;
                } 
            }
        }
	}else {
        if (add_ipv6_uc_routes==1) {
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

                    /*Add a public IPV6 entry for jericho private-public tests*/
                    /*the out_vlan is incremented by 1 for public-forwarding*/
                    if (is_device_or_above(unit,JERICHO)) {
                        rc = add_route_ip6(unit, route0, mask, 0, egress_intf[1]);
                    }
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
        }
        if (rc != BCM_E_NONE) {
            printf("Error. add_route_ip6 failed: $rc\n", rc);
            return BCM_E_FAIL;
        }    
    }

    if (add_ipv6_uc_routes==1) {
        if (i == 0) 
    	    print_ipv6_addr("route", route0);
        else if (i == 1) 
    	    print_ipv6_addr("route", route1);
        else if (i == 2) 
    	    print_ipv6_addr("route", route2);
        else /* (i == 3) */
    	    print_ipv6_addr("route", route3);

        if (i != 4) {
          printf("                                                ");
          print_ipv6_addr("mask", mask);
        }
    }
    if  (i == 4) /*i == 4 MC*/
	    print_ipv6_addr("route", route4);
    if  (i == 5) /*i == 5 MC*/
	    print_ipv6_addr("route", route5);

 
  } /* for loop*/

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

  /*bshell(unit, "dbm bcm -api");  */
  return 0;
}


/*
 * packet will be routed from in_port to out-port 
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
int ipv6_host_fec_per_outrif(int unit, int in_port, int out_port){
    int rv;
    int ing_intf_in; 
    int ing_intf_out; 
    int fec[2];
    int encap_id[2];
    int flags = 0;
    int in_vlan = 2; 
    int out_vlan = 100;
    int vrf = 0;
    bcm_mac_t my_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01};
    bcm_ip6_t host;  
    
    host[15]= 0;
    host[14]= 0;
    host[13]= 0;
    host[12]= 0x12;
    host[11]= 0x7;
    host[10]= 0xdb;
    host[9] = 0;
    host[8] = 0;
    host[7] = 0;
    host[6] = 0x88;
    host[5] = 0;
    host[4] = 0x78;
    host[3] = 0;
    host[2] = 0x16;
    host[1] = 0;
    host[0] = 0x1;
    
    /*** create ingress router interface ***/
    rv = bcm_vlan_create(unit, in_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
      printf("failed to create vlan %d", in_vlan);
    }
    
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
      printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
      return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac_address;
    intf.my_lsb_mac = my_mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    intf.ttl_valid = 1;
    intf.ttl = 62;
    intf.qos_map_valid = 1;
    intf.qos_map_id = qos_map_id_l3_ingress_get(unit);
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;
    if (rv != BCM_E_NONE) {
      printf("Error, l3__intf_rif__create");
    }
    
    /*** create egress router interface ***/
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
      printf("failed to create vlan %d", out_vlan);
    }
    
    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE) {
      printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
      return rv;
    }
    
    intf.vsi = out_vlan;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
      printf("Error, l3__intf_rif__create");
    }
    
    /*** create egress object 1 ***/
    fec[0] = 0;
    encap_id[0] = 0;
    /* built FEC points to out-RIF and MAC address */
    create_l3_egress_s l3eg;

    l3eg.out_tunnel_or_rif = ing_intf_out;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac, 6);  
    l3eg.vlan   = out_vlan;
    l3eg.out_gport   = out_port;
    l3eg.arp_encap_id = encap_id[0];
    l3eg.fec_id = fec[0]; 
    l3eg.allocation_flags = flags;
    l3eg.l3_flags = flags;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
      printf("Error, create egress object, out_port=%d, \n", out_port);
      return rv;
    }
	encap_id[0] = l3eg.arp_encap_id;
    fec[0] = l3eg.fec_id;
    printf("created FEC-id =0x%08x, \n", fec[0]);
    printf("next hop mac at encap-id %08x, \n", encap_id[0]);
    
    /*** add IPV6 host entry with OUT-RIF + MAC + eg-port ***/
    add_ipv6_host(unit, host, vrf, fec[0], encap_id[0]);

    print_host_ipv6("added ", host, vrf);
    printf("---> egress-intf=0x%08x, egress-mac: port=%d, \n", ing_intf_out, out_port);
    
    return rv;
}
 
int ipv6_fap_set_default_intf_as_ip_route(int unit){
    ipv6_fap_default_intf = ip_route_fec;
    return BCM_E_NONE;
}

int route_ipv6_traverse_scale(int unit, int index, bcm_l3_route_t *info, void *user_data)
{
    if (index > 128/4+1) {
        printf ("route_ipv6_traverse_scale, index out of range.\n");
        traverse_ipv6_rc = BCM_E_FAIL;
        return traverse_ipv6_rc;
    }
    if (info->l3a_intf != l3rts_ipv6_traverse[index].l3a_intf) {
        printf ("route_ipv6_traverse_scale, l3a_intf: Expected 0x%x, entry 0x%x\n", l3rts_ipv6_traverse[index].l3a_intf, info->l3a_intf);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    if (info->l3a_ip_mask != l3rts_ipv6_traverse[index].l3a_ip_mask) {
        printf ("route_ipv6_traverse_scale, l3a_ip_mask: Expected 0x%x, entry 0x%x\n", l3rts_ipv6_traverse[index].l3a_ip_mask, info->l3a_ip_mask);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    if (info->l3a_subnet != l3rts_ipv6_traverse[index].l3a_subnet) {
        printf ("route_ipv6_traverse_scale, l3a_subnet: Expected 0x%x, entry 0x%x\n", l3rts_ipv6_traverse[index].l3a_subnet, info->l3a_subnet);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    if (info->l3a_vrf != l3rts_ipv6_traverse[index].l3a_vrf) {
        printf ("route_ipv6_traverse_scale, l3a_vrf: Expected %d, entry %d\n", l3rts_ipv6_traverse[index].l3a_vrf, info->l3a_vrf);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    if (info->l3a_flags2 != l3rts_ipv6_traverse[index].l3a_flags2) {
        printf ("route_ipv6_traverse_scale, l3a_flags2: Expected 0x%x, entry 0x%x\n", l3rts_ipv6_traverse[index].l3a_flags2, info->l3a_flags2);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    int i;
    for (i=0; i<16; i++) {
        if (info->l3a_ip6_net[i] != l3rts_ipv6_traverse[index].l3a_ip6_net[i]) {
            printf ("route_ipv6_traverse_scale, l3a_ip6_net[%d]: Expected 0x%x, entry 0x%x\n", i, l3rts_ipv6_traverse[index].l3a_ip6_net[i], info->l3a_ip6_net[i]);
            traverse_ipv6_rc = BCM_E_FAIL;
        }
        if (info->l3a_ip6_mask[i] != l3rts_ipv6_traverse[index].l3a_ip6_mask[i]) {
            printf ("route_ipv6_traverse_scale, l3a_ip6_mask[%d]: Expected 0x%x, entry 0x%x\n", i, l3rts_ipv6_traverse[index].l3a_ip6_mask[i], info->l3a_ip6_mask[i]);
            traverse_ipv6_rc = BCM_E_FAIL;
        }
    }

    traverse_ipv6_route_counter++;

    printf ("Route traverse index: %d verified\n", index);
    return 0;
}

int host_ipv6_traverse_scale(int unit, int index, bcm_l3_host_t *info, void *user_data)
{
    if (index > 2) {
        printf ("host_ipv6_traverse_scale, index out of range.\n");
        traverse_ipv6_rc = BCM_E_FAIL;
        return traverse_ipv6_rc;
    }

    if (info->l3a_intf != l3hosts_ipv6_traverse[index].l3a_intf) {
        printf ("host_ipv6_traverse_scale, l3a_intf: Expected 0x%x, entry 0x%x\n", l3hosts_ipv6_traverse[index].l3a_intf, info->l3a_intf);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    if (info->l3a_ip_addr != l3hosts_ipv6_traverse[index].l3a_ip_addr) {
        printf ("host_ipv6_traverse_scale, l3a_ip_addr: Expected 0x%x, entry 0x%x\n", l3hosts_ipv6_traverse[index].l3a_ip_addr, info->l3a_ip_addr);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    if (info->l3a_vrf != l3hosts_ipv6_traverse[index].l3a_vrf) {
        printf ("host_ipv6_traverse_scale, l3a_vrf: Expected %d, entry %d\n", l3hosts_ipv6_traverse[index].l3a_vrf, info->l3a_vrf);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    if (info->l3a_flags2 != l3hosts_ipv6_traverse[index].l3a_flags2) {
        printf ("host_ipv6_traverse_scale, l3a_flags2: Expected 0x%x, entry 0x%x\n", l3hosts_ipv6_traverse[index].l3a_flags2, info->l3a_flags2);
        traverse_ipv6_rc = BCM_E_FAIL;
    }

    int i;
    for (i=0; i<16; i++) {
        if (info->l3a_ip6_addr[i] != l3hosts_ipv6_traverse[index].l3a_ip6_addr[i]) {
            printf ("host_ipv6_traverse_scale, l3a_ip6_addr[%d]: Expected 0x%x, entry 0x%x\n", i, l3hosts_ipv6_traverse[index].l3a_ip6_addr[i], info->l3a_ip6_addr[i]);
            traverse_ipv6_rc = BCM_E_FAIL;
        }
    }

    traverse_ipv6_host_counter++;

    printf ("Host traverse index: %d verified.\n", index);
    return 0;
}

void ipv6_create_mask(bcm_ip6_t *ip6_mask, uint32 prefix_length) {
    int i;

    for (i=15; i>=0; i--) {
        if (i < prefix_length/8) {
            ip6_mask[i] = 0xFF;
        } else if (i == prefix_length/8) {
            ip6_mask[i] = 0xFF - ((1 << (8 -(prefix_length % 8))) - 1);
        } else {
            ip6_mask[i] = 0;
        }
    }
}

void ipv6_and_mask(bcm_ip6_t *ip6_output, bcm_ip6_t *ip6, bcm_ip6_t *ip6_mask) {
    int i;
    for (i=0; i<16; i++) {
        ip6_output[i] = ip6[i] & ip6_mask[i];
    }
    
}

/*
 * The purpose of this function is to make sure that the first byte of a ipv6 is 0xFF
 * to make sure that it is a valid IPv6 MC address.
 */

void ipv6_and_mask_mc(bcm_ip6_t *ip6_output, bcm_ip6_t *ip6, bcm_ip6_t *ip6_mask) {
    int i;
    /*Make sure that the first byte of the DIP is 0xFF so that it is a valid MC DIP.*/
    ip6_output[0] = 0xFF;
    for (i=1; i<16; i++) {
        ip6_output[i] = ip6[i] & ip6_mask[i];
    }
}

/*
 *  The purpose of this cint is to create:
 *  35 interfaces (at the most) with different VRFs and MACs
 *  35 hosts/routes (at the most) with the same IPV6 DIP and VRF, but different prefix length.
 *      Prefix lengthes prefix_length_long and prefix_length_short go to LEM host. (interfaces 0 and 1)
 *      Prefix lengths 0,4,..prefix_length_short and prefix_length_long,prefix_length_long+4,..,128 go to KAPS route. (interfaces 2 and onwards)
 */
int NOF_SCALE_IPV6_INTERFACES = 128/4+1+2; /* Max number of interfaces */
bcm_l3_host_t  l3hosts_ipv6_traverse[2];
bcm_l3_route_t l3rts_ipv6_traverse[128/4+1];
int traverse_ipv6_rc = 0;
int traverse_ipv6_route_counter = 0;
int traverse_ipv6_host_counter = 0;

int ipv6_uc_fib_scale(int unit, int port0, int port1, int port2, int port3, int prefix_length_long, int prefix_length_short) {
    int rc,i;
    bcm_l3_intf_t l3if;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_mac_t mac_l3_ingress_add = {0x00, 0x00, 0x00, 0x00, 0x55, 0x00};
    bcm_mac_t mac_l3_engress_add = {0x00, 0x00, 0x00, 0x00, 0x66, 0x00};

    /* The long lem value is stored in 0 */
    /* The short lem value is stored in 1 */
    /* The lpm values are stored in 2..NOF_INTERFACES, from longest prefix to shortest */
    int vlan[NOF_SCALE_IPV6_INTERFACES];/*Each interface VLAN and VRF */
    uint8 mac_last_byte[NOF_SCALE_IPV6_INTERFACES];/*All interfaces has the same my MAC/dest MAC differ only in the last byte */
    int egress_intf[NOF_SCALE_IPV6_INTERFACES];
    int ports[NOF_SCALE_IPV6_INTERFACES];

    /*Set each interface parameters*/
    vlan[0] = 2000;
    mac_last_byte[0] = 0x10;
    for (i=1; i < NOF_SCALE_IPV6_INTERFACES; i++) {
        vlan[i] = vlan[i-1]+10;
        mac_last_byte[i] = (mac_last_byte[i-1]+0x5) % 0x100;
    }

    ports[0] = port0;
    ports[1] = port1;
    ports[2] = port2;
    ports[3] = port3;

    for (i=0; i < NOF_SCALE_IPV6_INTERFACES; i++) {
        /*Create L3 interface*/
        bcm_l3_intf_t_init(l3if);
        if(rc == BCM_E_EXISTS) {
          printf ("vlan %d aleady exist \n", vlan[i]);
        }
        mac_l3_ingress_add[5] = mac_last_byte[i];
        sal_memcpy(l3if.l3a_mac_addr, mac_l3_ingress_add, 6);
        l3if.l3a_vid = vlan[i];
        l3if.l3a_vrf = vlan[i];
        rc = bcm_l3_intf_create(unit, &l3if);
        if (rc != BCM_E_NONE) {
            return rc;
        }

        /*Create L3 egress*/
        bcm_l3_egress_t_init(&l3eg);
        l3eg.intf = l3if.l3a_intf_id;

        mac_l3_engress_add[5] = mac_last_byte[i];
        sal_memcpy(l3eg.mac_addr, mac_l3_engress_add, 6);
        l3eg.vlan   = vlan[i];
        l3eg.port   = ports[i % 4];

        rc = bcm_l3_egress_create(unit, 0, &l3eg, &l3egid);
        if (rc != BCM_E_NONE) {
          printf ("bcm_l3_egress_create failed \n");
          return rc;
        }

        egress_intf[i] = l3egid;
    }

    bcm_ip6_t ip6;
    ip6[0] = 0x12;
    ip6[1] = 0x34;
    ip6[2] = 0x56;
    ip6[3] = 0x78;
    ip6[4] = 0x9a;
    ip6[5] = 0xbc;
    ip6[6] = 0xde;
    ip6[7] = 0xf0;
    ip6[8] = 0x10;
    ip6[9] = 0x20;
    ip6[10] = 0x30;
    ip6[11] = 0x40;
    ip6[12] = 0x50;
    ip6[13] = 0x60;
    ip6[14] = 0x70;
    ip6[15] = 0x80;
    bcm_ip6_t ip6_mask;

    /*Create long prefix Host*/
    bcm_l3_host_t_init(&l3hosts_ipv6_traverse[0]);
    ipv6_create_mask(ip6_mask, prefix_length_long);
    ipv6_and_mask(l3hosts_ipv6_traverse[0].l3a_ip6_addr, ip6, ip6_mask);
    l3hosts_ipv6_traverse[0].l3a_vrf = vlan[0];
    l3hosts_ipv6_traverse[0].l3a_intf = egress_intf[0]; /* fec */
    l3hosts_ipv6_traverse[0].l3a_flags = BCM_L3_IP6;
    rc = bcm_l3_host_add(unit, &l3hosts_ipv6_traverse[0]);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_host_add failed: %d \n", rc);
      return rc;
    }

    /*Create short prefix Host*/
    bcm_l3_host_t_init(&l3hosts_ipv6_traverse[1]);
    ipv6_create_mask(ip6_mask, prefix_length_short);
    ipv6_and_mask(l3hosts_ipv6_traverse[1].l3a_ip6_addr, ip6, ip6_mask);
    l3hosts_ipv6_traverse[1].l3a_vrf = vlan[0];
    l3hosts_ipv6_traverse[1].l3a_intf = egress_intf[1]; /* fec */
    l3hosts_ipv6_traverse[1].l3a_flags = BCM_L3_IP6;
    l3hosts_ipv6_traverse[1].l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
    rc = bcm_l3_host_add(unit, &l3hosts_ipv6_traverse[1]);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_host_add failed: %d \n", rc);
      return rc;
    }

    int interface_counter = 0;
    int prefix_length;

    for (prefix_length=128; prefix_length >= 0; prefix_length-=4) {
        /* Skip invalid prefix lengths */
        if ((prefix_length > prefix_length_short) && (prefix_length < prefix_length_long)) {
            continue;
        }

        bcm_l3_route_t_init(&l3rts_ipv6_traverse[interface_counter]);
        ipv6_create_mask(l3rts_ipv6_traverse[interface_counter].l3a_ip6_mask, prefix_length);
        ipv6_and_mask(l3rts_ipv6_traverse[interface_counter].l3a_ip6_net, ip6, l3rts_ipv6_traverse[interface_counter].l3a_ip6_mask);
        l3rts_ipv6_traverse[interface_counter].l3a_vrf = vlan[0];
        l3rts_ipv6_traverse[interface_counter].l3a_intf = egress_intf[interface_counter+2]; /* fec */
        l3rts_ipv6_traverse[interface_counter].l3a_flags = BCM_L3_IP6;
        l3rts_ipv6_traverse[interface_counter].l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
        rc = bcm_l3_route_add(unit, l3rts_ipv6_traverse[interface_counter]);
        if (rc != BCM_E_NONE) {
          printf ("bcm_l3_route_add failed: %d \n", rc);
          return rc;
        }
        interface_counter++;
    }

    rc = bcm_l3_route_traverse(unit, BCM_L3_IP6, 0, 0, route_ipv6_traverse_scale, NULL);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_route_traverse failed: %d \n", rc);
      return rc;
    }

    if ((traverse_ipv6_rc != BCM_E_NONE) || (traverse_ipv6_route_counter != interface_counter)) {
      printf ("bcm_l3_route_traverse failed: rc=%d, actual_counter=%d, expected counter=%d.\n", traverse_ipv6_rc, traverse_ipv6_route_counter, interface_counter);
      return traverse_ipv6_rc;
    }

    rc = bcm_l3_host_traverse(unit, BCM_L3_IP6, 0, 0, host_ipv6_traverse_scale, NULL);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_host_traverse failed: %d \n", rc);
      return rc;
    }

    if ((traverse_ipv6_rc != BCM_E_NONE) || (traverse_ipv6_host_counter != 2)) {
      printf ("bcm_l3_host_traverse failed: rc=%d, actual_counter=%d, expected counter=%d.\n", traverse_ipv6_rc, traverse_ipv6_host_counter, 2);
      return traverse_ipv6_rc;
    }

    return rc;
}

void ipv6_print(bcm_ip6_t *ipv6) {
    int i;
    printf("%02X%02X", ipv6[0], ipv6[1]);
    for(i = 2; i < 16; i += 2) {
        printf(":%02X%02X", ipv6[i], ipv6[i+1]);
    }
}


int entries = 3072;
int host_check_list[3072] = {0};
int route_check_list[3072] = {0};
int host_counter = 0;
int route_counter = 0;

int ipv6_kbp_host_traverse_cb(int unit, int index, bcm_l3_host_t *info, void *data){

	int         i;
	int         *check_list;
    int         check_index;
	uint32      expected_fec = 0x20001000;
	bcm_ip6_t   base_address;

    base_address[0] = 0x0a;
    check_list = host_check_list;

	if(info->l3a_ip6_addr[0] != base_address[0]) {
		printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_addr);
        printf(", expected=");
        ipv6_print(base_address);
        printf("\n");
		return BCM_E_FAIL;
	}
    check_index = info->l3a_ip6_addr[14] * 0x100 + info->l3a_ip6_addr[15];
	if(check_list[check_index] == 1) {
		printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_addr);
        printf(", 2nd check_list hit on cell=%d\n", check_index);
		return BCM_E_FAIL;
	}
    else {
		check_list[check_index] = 1;
	}
	if(info->l3a_vrf != info->l3a_ip6_addr[14]) {
		printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_addr);
        printf(", vrf=%d. VRF matching error, expected=%d\n", info->l3a_vrf, info->l3a_ip6_addr[14]);
		return BCM_E_FAIL;
	}
	if(info->l3a_intf != expected_fec) {
		printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_addr);
		printf(", info->l3a_intf=0x%08x. fec matching error, expected=0x%08x\n", info->l3a_intf, expected_fec);
		return BCM_E_FAIL;
	}

	host_counter++;
	if((host_counter & 0xFF) == 0) {
		printf("Host traverse index: %d verified\n", index);
	}

	if(index == entries - 1) {
		printf("TRAVERSED ALL HOSTS!!\n");
		printf("check the hosts checking list:\n");
		for(i = 0; i < entries; i++) {
			if(host_check_list[i] != 1) {
				printf("Error: host_check_list[%d]=%d\n", i, host_check_list[i]);
				return BCM_E_FAIL;
			}
		}
		printf("verified host checking lists\n");
	}
	return 0;
}

int ipv6_kbp_route_traverse_cb(int unit, int index, bcm_l3_route_t *info, void *data){

    int         i;
	int         *check_list;
    int         check_index;
	uint32      expected_fec = 0x20001001;
	bcm_ip6_t   base_address;

    base_address[0] = 0x0b;
    check_list = route_check_list;

	if(info->l3a_ip6_net[0] != base_address[0]) {
		printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_net);
        printf(", expected=");
        ipv6_print(base_address);
        printf("\n");
		return BCM_E_FAIL;
	}
    check_index = info->l3a_ip6_net[13] * 0x100 + info->l3a_ip6_net[14];
	if(check_list[check_index] == 1) {
		printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_net);
        printf(", 2nd check_list hit on cell=%d\n", check_index);
		return BCM_E_FAIL;
	}
    else {
		check_list[check_index] = 1;
	}
	if(info->l3a_vrf != info->l3a_ip6_net[13]) {
		printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_addr);
        printf(", vrf=%d. VRF matching error, expected=%d\n", info->l3a_vrf, info->l3a_ip6_net[13]);
		return BCM_E_FAIL;				
	}
	if(info->l3a_intf != expected_fec) {
		printf("Error: index=%d, ip6_address=", index);
        ipv6_print(info->l3a_ip6_net);
		printf(", info->l3a_intf=0x%08x. fec matching error, expected=0x%08x\n", info->l3a_intf, expected_fec);
		return BCM_E_FAIL;
	}

	route_counter++;
	if((route_counter & 0xFF) == 0) {
		printf ("Route traverse index: %d verified\n", index);
	}

	if(index == entries - 1) {
		printf("TRAVERSED ALL ROUTES!!\n");
		printf("check the route checking list\n");
		for(i = 0; i < entries; i++) {
			if (route_check_list[i] != 1) {
				printf("Error: route_check_list[%d]=%d\n", i, route_check_list[i]);
				return BCM_E_FAIL;
			}
		}
		printf("verified route checking list\n");
	}
	return 0;
}

int ipv6_kbp_traverse(int unit) {

    int i, rv = 0;
	int vrf = 0;

	uint32 fec_host = 0x20001000;
	uint32 fec_route = 0x20001001;

    bcm_ip6_t host;
	bcm_ip6_t route;
    bcm_ip6_t mask;
    
    bcm_l3_host_t  l3host;
	bcm_l3_route_t l3route;

    host[0] = 0x0a;
    route[0] = 0x0b;
    for(i = 0; i < 15; i++) {
        mask[i] = 0xff;
    }


    /*Adding 4K hosts, 4K routes*/
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr, host, 16);
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec_host;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask, 16);
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = fec_route;

    printf("Start adding %d hosts and %d routes\n", entries, entries);
    for(i = 0; i < entries; i++) {
         
		rv = bcm_l3_host_add(unit, &l3host);
		if(rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, entry %d, l3a_ip6_addr=", i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_add(unit, &l3route);
        if(rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_add Failed, entry %d, l3a_ip6_net=", i);
             ipv6_print(l3host.l3a_ip6_net);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        if(l3host.l3a_ip6_addr[15] == 0xff) {
            l3host.l3a_ip6_addr[15] = 0x00;
            l3host.l3a_ip6_addr[14]++;
			l3host.l3a_vrf++;
			l3route.l3a_vrf++;
        }
        else {
            l3host.l3a_ip6_addr[15]++;
        }

        if(l3route.l3a_ip6_net[14] == 0xff) {
            l3route.l3a_ip6_net[14] = 0x00;
            l3route.l3a_ip6_net[13]++;
        }
        else {
            l3route.l3a_ip6_net[14]++;
        }
    }

    printf("Adding done\nStart traversing\n");
	rv = bcm_l3_host_traverse(unit, BCM_L3_IP6, 0, 0, ipv6_kbp_host_traverse_cb, NULL);
	if(rv != BCM_E_NONE) {
		 printf("Error, bcm_l3_host_traverse \n");
		 return rv;
	}
	if(host_counter != entries) {
		 printf("Error, host_counter = %d, expected %d \n", host_counter, entries*2);
		 return -1;
	}
	rv = bcm_l3_route_traverse(unit, BCM_L3_IP6, 0, 0, ipv6_kbp_route_traverse_cb, NULL);
	if(rv != BCM_E_NONE) {
		 printf("Error, bcm_l3_route_traverse \n");
		 return rv;
	}
	if(route_counter != entries) {
		 printf("Error, route_counter = %d, expected %d \n", route_counter, entries*2);
		 return -1;
	}

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr, host, 16);
    l3host.l3a_vrf = vrf;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask, 16);
    l3route.l3a_vrf = vrf;

    printf("Traversing done\nStart deleting entries\n");
    for(i = 0; i < entries; i++) {        

		rv = bcm_l3_host_delete(unit, &l3host);
		if(rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_delete Failed, entry %d, l3a_ip6_addr=", i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_delete(unit, &l3route);
        if(rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_delete Failed, entry %d, l3a_ip6_net=", i);
             ipv6_print(l3route.l3a_ip6_net);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        if(l3host.l3a_ip6_addr[15] == 0xff) {
            l3host.l3a_ip6_addr[15] = 0x00;
            l3host.l3a_ip6_addr[14]++;
			l3host.l3a_vrf++;
			l3route.l3a_vrf++;
        }
        else {
            l3host.l3a_ip6_addr[15]++;
        }

        if(l3route.l3a_ip6_net[14] == 0xff) {
            l3route.l3a_ip6_net[14] = 0x00;
            l3route.l3a_ip6_net[13]++;
        }
        else {
            l3route.l3a_ip6_net[14]++;
        }
    }

    printf("Deleting entries done\n");
	return rv;
}


int ipv6_kbp_semantic(int unit) {

    int i, rv = 0;
	int vrf = 0;

	uint32 fec_host = 0x20001000;
	uint32 fec_route = 0x20001001;


    bcm_ip6_t host;
	bcm_ip6_t route;
    bcm_ip6_t mask;

    bcm_l3_host_t  l3host;
	bcm_l3_route_t l3route;

    host[0] = 0x0a;
    route[0] = 0x0b;
    for(i = 0; i < 15; i++) {
        mask[i] = 0xff;
    }

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr, host, 16);
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec_host;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask, 16);
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = fec_route;

    printf("entries add - start\n");

    for (i = 1; i <= 16; i++) {    
            
		rv = bcm_l3_host_add(unit, &l3host);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, entry %d, l3a_ip6_addr=", i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_add(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_add Failed, entry %d, l3a_ip6_net=", i);
             ipv6_print(l3host.l3a_ip6_net);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

		l3host.l3a_vrf++;
		l3route.l3a_vrf += 4;
        l3host.l3a_ip6_addr[15]++;
		l3route.l3a_ip6_net[14]++;
    }
    printf("entries add - end\n");

	bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr, host, 16);
    l3host.l3a_vrf = vrf;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask, 16);
    l3route.l3a_vrf = vrf;

    printf("entries get - start\n");
    for (i = 1; i <= 16; i++) {

		rv = bcm_l3_host_find(unit, &l3host);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_find Failed, entry %d, l3a_ip6_addr=", i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf(", rv=0x%x\n", rv);
             return rv;
        }
        if (l3host.l3a_intf != fec_host) {
             printf("Error, host: fec Failed, expected: 0x%08x, actual: 0x%08x\nentry %d, l3a_ip6_addr=", fec_host, l3host.l3a_intf, i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf("\n");
             return rv;
        }

        rv = bcm_l3_route_get(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_get Failed, entry %d, l3a_ip6_net=", i);
             ipv6_print(l3host.l3a_ip6_net);
             printf(", rv=0x%x\n", rv);
             return rv;
        }
        if (l3route.l3a_intf != fec_route) {
             printf("Error, route: fec Failed, expected: 0x%08x, actual: 0x%08x\nentry %d, l3a_ip6_net=", fec_route, l3route.l3a_intf, i);
             ipv6_print(l3host.l3a_ip6_net);
             printf("\n");
             return rv;
        }

        printf("Host - Entry %d: ip6 = ", i);
        ipv6_print(l3host.l3a_ip6_addr);
        printf(", vrf = %d, fec=0x%08x\n", l3host.l3a_vrf, l3host.l3a_intf);

        printf("Route- Entry %d: ip6 = ", i);
        ipv6_print(l3route.l3a_ip6_net);
        printf(", vrf = %d, fec=0x%08x, mask = ", l3route.l3a_vrf, l3route.l3a_intf);
        ipv6_print(l3route.l3a_ip6_mask);
        printf("\n");

		l3host.l3a_vrf++;
		l3route.l3a_vrf += 4;
        l3host.l3a_ip6_addr[15]++;
		l3route.l3a_ip6_net[14]++;
        l3host.l3a_intf = 0;
        l3host.encap_id = 0;
        l3route.l3a_intf = 0;
    }
    printf("entries get - end\n");

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr, host, 16);
    l3host.l3a_vrf = vrf;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask, 16);
    l3route.l3a_vrf = vrf;

    printf("entries delete - start\n");
    for (i = 1; i <= 16; i++) {

		rv = bcm_l3_host_delete(unit, &l3host);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_delete Failed, entry %d, l3a_ip6_addr=", i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_delete(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_delete Failed, entry %d, l3a_ip6_net=", i);
             ipv6_print(l3host.l3a_ip6_net);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

		l3host.l3a_vrf++;
		l3route.l3a_vrf += 4;
        l3host.l3a_ip6_addr[15]++;
		l3route.l3a_ip6_net[14]++;
    }
    printf("entries delete - end\n");
    return 0;
}

int default_route_mc_flood_vlan_config(int unit, bcm_port_t ing_port, bcm_port_t egr_port, int vrf, int vlan, bcm_if_t *egr_id, int ip6) {
    int rv = BCM_E_NONE;
    
    bcm_mac_t mac_addr = { 0x00, 0x0a, 0x00, 0x00, 0x0b, 0x00 };
    bcm_mac_t mac_mask = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_mac_t dest_mac = { 0x22, 0x00, 0x00, 0x00, 0x00, 0x22 };
    bcm_l2_station_t station;
    int station_id;
    bcm_vlan_port_t vlan_port;
    bcm_l3_intf_t intf;
    bcm_if_t ing_if;
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_egress_t egr;
    bcm_gport_t ingress_port, egress_port;

    BCM_GPORT_SYSTEM_PORT_ID_SET(ingress_port, ing_port);
    BCM_GPORT_SYSTEM_PORT_ID_SET(egress_port, egr_port);


    bcm_port_tpid_delete_all(unit, egress_port);
    rv = bcm_port_tpid_add(unit, egress_port, 0x8100, 0);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_port_tpid_add: %d\n", rv);
        return rv;
    }
    rv = bcm_port_tpid_add(unit, egress_port, 0x88a8, 0);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_port_tpid_add: %d\n", rv);
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, ingress_port, vlan);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_port_untagged_vlan_set: %d\n", rv);
        return rv;
    }

    bcm_l2_station_t_init(&station);
    sal_memcpy(station.dst_mac, mac_addr, 6);
    sal_memcpy(station.dst_mac_mask, mac_mask, 6);
    rv = bcm_l2_station_add(unit, &station_id, &station);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_l2_station_add: %d\n", rv);
        return rv;
    }

    bcm_vlan_t ing_vsi = 2;
    bcm_vswitch_destroy(unit, ing_vsi);
    rv = bcm_vswitch_create_with_id(unit, ing_vsi);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_create ing_vsi: %d\n", rv);
        return rv;
    }
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
    vlan_port.port = ingress_port;
    vlan_port.match_vlan = vlan;
    vlan_port.egress_vlan = vlan;
    vlan_port.vsi = ing_vsi;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create: %d\n", rv);
        return rv;
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_flags = BCM_L3_WITH_ID;
    intf.l3a_vid = ing_vsi;
    intf.l3a_intf_id = ing_vsi;
    intf.l3a_vrf = vrf;
    sal_memcpy(intf.l3a_mac_addr, mac_addr, 6);
    rv = bcm_l3_intf_create(unit, &intf);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_l3_intf_create ingress: %d\n", rv);
        return rv;
    }
    ing_if = intf.l3a_intf_id;

    bcm_vlan_t egr_vsi = 3;
    bcm_vswitch_destroy(unit, egr_vsi);
    rv = bcm_vswitch_create_with_id(unit, egr_vsi);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_create egr_vsi: %d\n", rv);
        return rv;
    }
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
    vlan_port.port = egress_port;
    vlan_port.match_vlan = vlan;
    vlan_port.egress_vlan = vlan;
    vlan_port.vsi = ing_vsi;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create: %d\n", rv);
        return rv;
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_flags = BCM_L3_WITH_ID;
    intf.l3a_vid = egr_vsi;
    intf.l3a_intf_id = egr_vsi;
    intf.l3a_vrf = vrf;
    sal_memcpy(intf.l3a_mac_addr, mac_addr, 6);
    rv = bcm_l3_intf_create(unit, &intf);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_l3_intf_create egress: %d\n", rv);
        return rv;
    }

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_intf.vrf = vrf;
    if(vrf == 0) {
        ingress_intf.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
    }
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &ing_if);
    if(rv) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    bcm_l3_egress_t_init(&egr);
    egr.intf = egr_vsi;
    sal_memcpy(egr.mac_addr, dest_mac, 6);
    egr.port = egress_port;
    egr.vlan = vlan;
    rv = bcm_l3_egress_create(unit, 0, &egr, egr_id);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_l3_egress_create: %d\n", rv);
        return rv;
    }

    bcm_l3_route_t route;
    bcm_ip6_t ip6_addr = { 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x77, 0x77, 0x88, 0x88 };
    bcm_ip6_t ip6_mask = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 };

    bcm_l3_route_t_init(&route);
    route.l3a_flags = ip6 ? BCM_L3_IP6 : 0;
    route.l3a_subnet = 0x0a141e28;
    route.l3a_ip_mask = 0xffffff00;
    route.l3a_ip6_net = ip6_addr;
    route.l3a_ip6_mask = ip6_mask;
    route.l3a_vrf = vrf;
    route.l3a_intf = *egr_id;

    rv = bcm_l3_route_add(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_l3_route_add: %d\n", rv);
        return rv;
    }

    return rv;
}

int default_route_add(int unit, int vrf, int intf_id, int ip6) {
    int rv = BCM_E_NONE;
    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);

    route.l3a_flags = ip6 ? BCM_L3_IP6 : 0;

    route.l3a_subnet = 0x00000000;
    route.l3a_ip_mask = 0x00000000;
    sal_memset(route.l3a_ip6_net, 0x00, 16);
    sal_memset(route.l3a_ip6_mask, 0x00, 16);
    route.l3a_vrf = vrf;
    route.l3a_intf = intf_id;

    rv = bcm_l3_route_add(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_l3_route_add: %d\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

int ipv6_kbp_public_private_semantic(int unit) {

    int i = 0;
    int rv = BCM_E_NONE;

    uint32 fec_host = 0x20001000;
    uint32 fec_route = 0x20001001;

    bcm_ip6_t host;
    bcm_ip6_t route;
    bcm_ip6_t mask;

    bcm_l3_host_t  l3host;
    bcm_l3_route_t l3route;

    host[0] = 0x0a;
    route[0] = 0x0b;
    for(i = 0; i < 15; i++) {
        mask[i] = 0xff;
    }

    /* bcm_l3_host_add/find/delete can not add/get/delete
            public entry to the LEM becase enhanced_fib_scale_prefix_length_ipv6_long
            requires IPv6 UC routes to stored in the KAPS, disable ELK soc property */

    /* verify bcm_l3_host_add/find/delete route for public entry */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr, host, 16);
    l3host.l3a_vrf = 0;    /* host to public route */
    l3host.l3a_intf = fec_host;

    /* verify bcm_l3_route_add/get/delete for public entry */
    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask, 16);
    l3route.l3a_vrf = 0;  /* public route */
    l3route.l3a_intf = fec_route;

    printf("entries add - start\n");

    for (i = 1; i <= 16; i++) {
        rv = bcm_l3_host_add(unit, &l3host);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, entry %d, l3a_ip6_addr=", i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_add(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_add Failed, entry %d, l3a_ip6_net=", i);
             ipv6_print(l3host.l3a_ip6_net);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        l3host.l3a_ip6_addr[15]++;
        l3route.l3a_ip6_net[14]++;
    }
    printf("entries add - end\n");

    printf("iterator all entries afer add \n");
    bshell(unit, "diag pp kbp iterator ");

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr, host, 16);
    l3host.l3a_vrf = 0;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask, 16);
    l3route.l3a_vrf = 0;

    printf("entries get - start\n");
    for (i = 1; i <= 16; i++) {

    rv = bcm_l3_host_find(unit, &l3host);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_find Failed, entry %d, l3a_ip6_addr=", i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf(", rv=0x%x\n", rv);
             return rv;
        }
        if (l3host.l3a_intf != fec_host) {
             printf("Error, host: fec Failed, expected: 0x%08x, actual: 0x%08x\nentry %d, l3a_ip6_addr=", fec_host, l3host.l3a_intf, i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf("\n");
             return rv;
        }

        rv = bcm_l3_route_get(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_get Failed, entry %d, l3a_ip6_net=", i);
             ipv6_print(l3host.l3a_ip6_net);
             printf(", rv=0x%x\n", rv);
             return rv;
        }
        if (l3route.l3a_intf != fec_route) {
             printf("Error, route: fec Failed, expected: 0x%08x, actual: 0x%08x\nentry %d, l3a_ip6_net=", fec_route, l3route.l3a_intf, i);
             ipv6_print(l3host.l3a_ip6_net);
             printf("\n");
             return rv;
        }

        printf("Host - Entry %d: ip6 = ", i);
        ipv6_print(l3host.l3a_ip6_addr);
        printf(", vrf = %d, fec=0x%08x\n", l3host.l3a_vrf, l3host.l3a_intf);

        printf("Route- Entry %d: ip6 = ", i);
        ipv6_print(l3route.l3a_ip6_net);
        printf(", vrf = %d, fec=0x%08x, mask = ", l3route.l3a_vrf, l3route.l3a_intf);
        ipv6_print(l3route.l3a_ip6_mask);
        printf("\n");

        l3host.l3a_ip6_addr[15]++;
        l3route.l3a_ip6_net[14]++;
        l3host.l3a_intf = 0;
        l3host.encap_id = 0;
        l3route.l3a_intf = 0;
    }
    printf("entries get - end\n");

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3host.l3a_ip6_addr, host, 16);
    l3host.l3a_vrf = 0;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(l3route.l3a_ip6_net, route, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask, 16);
    l3route.l3a_vrf = 0;

    printf("entries delete - start\n");
    for (i = 1; i <= 16; i++) {
        rv = bcm_l3_host_delete(unit, &l3host);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_delete Failed, entry %d, l3a_ip6_addr=", i);
             ipv6_print(l3host.l3a_ip6_addr);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_delete(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_delete Failed, entry %d, l3a_ip6_net=", i);
             ipv6_print(l3host.l3a_ip6_net);
             printf(", rv=0x%x\n", rv);
             return rv;
        }

        l3host.l3a_ip6_addr[15]++;
        l3route.l3a_ip6_net[14]++;
    }
    printf("entries delete - end\n");

    printf("iterator all entries afer delete \n");
    bshell(unit, "diag pp kbp iterator ");

    return 0;
}
