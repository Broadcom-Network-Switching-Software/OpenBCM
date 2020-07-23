/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/**
 * Example for configuring Equal-Cost Multi-Path (ECMP) Routing. 
 *  
 * Introduction 
 *  
 *   ECMPs are used for load-balancing packets among several FECs. 
 *   An ECMP is composed of a group of FECs. 
 *   ECMPs can be configured as a destination, for example as IP routes. 
 *   When a packet is sent to this route, the device will perform hashing  
 *   on (some part of) the packet to decide on which FEC it should be sent. 
 *   In this way packets can be load-balanced among several paths.
 *  
 * Running 
     cint utility\cint_utils_l3.c
     cint cint_queue_tests.c
     cint cint_l3_multipath.c
     l3_multipath_run_with_defaults_2(unit, unit2, inport);
 *  
 *  
 * Configuration
 *  
 *   The example configures routing for a stream.
 *   The destination of that stream is set to be an ECMP.
 *   The ECMP is configured to contain a group of FECs,
 *   with each FEC having a different destination port.
 *  
 *   Details:
 *    1. Setup ECMP hashing based on the Ethernet header of packets.
 *    2. Create an l3 ingress interface to setup routing for packets arriving on inport with
 *       vlan 12 and MAC 00:00:00:00:55:34.
 *    3. Create 4 egress interfaces (FECs) with destination ports 1-4.
 *    4. Create an ECMP group and add the FECs to it.
 *    5. Add route for 112.52.22.1, and set the ECMP as the destination.
 *  
 * Traffic: 
 *  
 *   Send packets from port inport with vlan 12, DA 00:00:00:00:55:34 and DIP 112.52.22.1.
 *   The device will route packets through one of the configured ports (1-4).
 *   To get the device to route to different ports, change the SA of the packet.
 */

int CMD_FAIL = -1;
int CINT_NO_FLAGS = 0;
int egress_intf[16];
int encap_id[16];
bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34};
bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};

struct cint_l3_multipath_cfg_s {
  int ecmp_api_is_ecmp;
  int ecmp_is_resilant_hashing;
};

cint_l3_multipath_cfg_s cint_l3_multipath_cfg = {
  1, /* ecmp_api_is_ecmp */
  0 /* ecmp_is_resilant_hashing */
};

int
add_route (int unit, bcm_if_t intf)
{
  bcm_l3_route_t ecmp_l3rt;
  uint32 route;
  int rc;

  route = ((112%192)<<24)+((52%192)<<16)+((22%192)<<8)+(1%192); /*112.52.22.1*/

  bcm_l3_route_t_init(&ecmp_l3rt);
  ecmp_l3rt.l3a_flags = 0;
  ecmp_l3rt.l3a_subnet = route;
  ecmp_l3rt.l3a_ip_mask = 0xfffff000;
  ecmp_l3rt.l3a_intf = intf;

  rc = bcm_l3_route_add(unit, &ecmp_l3rt);
  if (BCM_FAILURE(rc)) {
    printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
    return CMD_FAIL;
  }
  printf("L3 multipath route created, 0x%08x (0x%08x)\n", ecmp_l3rt.l3a_subnet, ecmp_l3rt.l3a_ip_mask);

  return rc;
}


int
l3_multipath_run(int unit, int unit_2nd, int in_port, int vlan, int nof_paths, int *out_port_arr, int *modid_arr)
{
  int ingress_intf;
  int rc;
  int i;
  int gport;
  bcm_l3_egress_ecmp_t ecmp;
  bcm_if_t ecmp_mpath_id;
  int core, tm_port;

   /* hash settings */
  rc = bcm_switch_control_set(unit,bcmSwitchECMPHashSrcPortEnable ,0);
  if (rc != BCM_E_NONE)
  {
    printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashSrcPortEnable  %d: %s\n", rc, bcm_errmsg(rc));    
    return rc;
  }
  rc = bcm_switch_control_set(unit,bcmSwitchECMPHashOffset ,1);
  if (rc != BCM_E_NONE)
  {
    printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashOffset  %d: %s\n", rc, bcm_errmsg(rc));    
    return rc;
    return rc;
  }
  rc = bcm_switch_control_set(unit,bcmSwitchECMPHashSeed ,1234);
  if (rc != BCM_E_NONE)
  {
    printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashSeed  %d: %s\n", rc, bcm_errmsg(rc));    
    return rc;
    return rc;
  }
  rc = bcm_switch_control_set(unit,bcmSwitchECMPHashConfig,BCM_HASH_CONFIG_CLOCK_BASED);
  if (rc != BCM_E_NONE)
  {
    printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashConfig  %d: %s\n", rc, bcm_errmsg(rc));    
    return rc;
    return rc;
  }
  rc = bcm_switch_control_port_set(unit, in_port, bcmSwitchECMPHashPktHeaderCount ,1);
  if (rc != BCM_E_NONE)
  {
    printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashPktHeaderCount  %d: %s\n", rc, bcm_errmsg(rc));    
    return rc;
    return rc;
  }
  if (unit_2nd >= 0)
  {
    rc = bcm_switch_control_set(unit_2nd,bcmSwitchECMPHashSrcPortEnable ,0);
    if (rc != BCM_E_NONE)
    {
      print rc;
      return rc;
    }
    rc = bcm_switch_control_set(unit_2nd,bcmSwitchECMPHashOffset ,1);
    if (rc != BCM_E_NONE)
    {
      print rc;
      return rc;
    }
    rc = bcm_switch_control_set(unit_2nd,bcmSwitchECMPHashSeed ,1234);
    if (rc != BCM_E_NONE)
    {
      print rc;
      return rc;
    }
    rc = bcm_switch_control_set(unit_2nd,bcmSwitchECMPHashConfig,BCM_HASH_CONFIG_CLOCK_BASED);
    if (rc != BCM_E_NONE)
    {
      print rc;
      return rc;
    }
    rc = bcm_switch_control_port_set(unit_2nd, in_port, bcmSwitchECMPHashPktHeaderCount ,1);
    if (rc != BCM_E_NONE)
    {
      print rc;
      return rc;
    }
  }
  
  /* Setup routing for packets arriving on in_port with vlan vlan (param) and MAC 00:00:00:00:55:34. */
  bcm_pbmp_t pbmp;
  bcm_l3_intf_t l3if, l3if_ori;

  /* create vlan */  
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, in_port);
  
  rc = bcm_vlan_create (unit, vlan);
  if (BCM_FAILURE(rc)) {
    printf("bcm_vlan_create %d: %s\n", rc, bcm_errmsg(rc));
    return CMD_FAIL;
  }

  rc = bcm_vlan_port_add(unit, vlan, pbmp, pbmp);
  if (BCM_FAILURE(rc)) {
    printf("bcm_vlan_port_add %d: %s\n", rc, bcm_errmsg(rc));
    return CMD_FAIL;
  }

  create_l3_intf_s intf;
  intf.vsi = vlan;
  intf.my_global_mac = mac_l3_ingress;
  intf.my_lsb_mac = mac_l3_ingress;
  intf.skip_mymac = 1;
  rc = l3__intf_rif__create(unit, &intf);
  ingress_intf = intf.rif;
  if (rc != BCM_E_NONE) {
  	printf("Error, l3__intf_rif__create");
  }

  /* Create FECs and set ports out_port_arr[i] to be their destinations. */
  for (i=0; i<nof_paths; i++)
  {
    rc = get_core_and_tm_port_from_port(unit, out_port_arr[i], &core, &tm_port);
    if (rc != BCM_E_NONE) {
        printf("Error, in get_core_and_tm_port_from_port\n");
        return rc;
    }    
    BCM_GPORT_MODPORT_SET(gport, modid_arr[i] + core, tm_port);
    /* it is assumed modid == unit id */
    create_l3_egress_s l3eg;
 
    l3eg.out_tunnel_or_rif = ingress_intf;
    sal_memcpy(l3eg.next_hop_mac_addr, &mac_l3_egress, 6);
    l3eg.vlan = vlan;
    l3eg.out_gport = gport;
    l3eg.arp_encap_id = encap_id[i];
    l3eg.fec_id = egress_intf[i];
    l3eg.allocation_flags = CINT_NO_FLAGS;
    rc = l3__egress__create(modid_arr[i],l3eg);
    if (BCM_FAILURE(rc)) {
    printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
    return CMD_FAIL;
    }
    printf("L3 egress created, MAC 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x on VLAN 0x%x\n",
    l3eg.next_hop_mac_addr[0], 
    l3eg.next_hop_mac_addr[1], 
    l3eg.next_hop_mac_addr[2], 
    l3eg.next_hop_mac_addr[3], 
    l3eg.next_hop_mac_addr[4], 
    l3eg.next_hop_mac_addr[5], vlan);
	
	encap_id[i] = l3eg.arp_encap_id;
    egress_intf[i] = l3eg.fec_id ;
    if (unit_2nd >=0)
    {
        l3eg.allocation_flags = BCM_L3_WITH_ID;
        rc = l3__egress__create(1-modid_arr[i],l3eg);
        if (BCM_FAILURE(rc)) {
        printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
        return CMD_FAIL;
        }
        printf("L3 egress created, MAC 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x on VLAN 0x%x\n",
        l3eg.next_hop_mac_addr[0], 
        l3eg.next_hop_mac_addr[1], 
        l3eg.next_hop_mac_addr[2], 
        l3eg.next_hop_mac_addr[3], 
        l3eg.next_hop_mac_addr[4], 
        l3eg.next_hop_mac_addr[5], vlan);
    }    
  }

  /* Create ECMP. */
  if (cint_l3_multipath_cfg.ecmp_api_is_ecmp) {
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.max_paths = nof_paths;

	if (cint_l3_multipath_cfg.ecmp_is_resilant_hashing) {
		ecmp.dynamic_mode = ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
	}

    /* create ECMP L3 egress */  
    rc = bcm_l3_egress_ecmp_create(unit, &ecmp, nof_paths, egress_intf);
    if (BCM_FAILURE(rc)) {
      printf("bcm_l3_egress_ecmp_create\n"); 
      printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
      return CMD_FAIL;
    }
    printf ("L3 egress multipath created: 0x%x\n", ecmp.ecmp_intf);
  } else {
    /* create multipath L3 egress (this API is deprecated - use bcm_l3_egress_ecmp_create instead). */
    rc = bcm_l3_egress_multipath_create(unit, CINT_NO_FLAGS, nof_paths, egress_intf, &ecmp_mpath_id);
    if (BCM_FAILURE(rc)) {
      printf("bcm_l3_egress_multipath_create\n"); 
      printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
      return CMD_FAIL;
    }
    printf ("L3 egress multipath created: 0x%x\n", ecmp_mpath_id);
  }

  /* add route to ecmp */
  if (cint_l3_multipath_cfg.ecmp_api_is_ecmp) {
    rc = add_route(unit,ecmp.ecmp_intf);
  } else {
    rc = add_route(unit,ecmp_mpath_id);
  }
  if (rc!=BCM_E_NONE)
  {
    printf("add_route\n"); 
    print rc;
    return rc;
  }

  return rc;
}

int
l3_multipath_run_with_defaults(int unit, int unit_2nd)
{
  int rc;
  int in_port = 1;
  int vlan = 12;
  int nof_paths = 4;
  int out_port_arr[4] = {2,3,4,5};
  int modid_arr_1_device[4] = {0,0,0,0};
  int modid_arr_2_devices[4] = {0,0,1,1};
  int *modid_arr;

  if (unit_2nd < 0)
  {
    modid_arr = modid_arr_1_device;
  } 
  else
  {
    modid_arr = modid_arr_2_devices;
  } 
  
  rc = l3_multipath_run(unit,unit_2nd,in_port,vlan,nof_paths,out_port_arr,modid_arr);
  return rc;
}

int
l3_multipath_run_with_defaults_2(int unit, int unit_2nd, int in_port)
{
  int rc;
  int vlan = 12;
  int nof_paths = 4;
  int out_port_arr[4] = {1,2,3,4};
  int modid_arr_1_device[4] = {0,0,0,0};
  int modid_arr_2_devices[4] = {0,0,1,1};
  int *modid_arr;

  if (unit_2nd < 0)
  {
    modid_arr = modid_arr_1_device;
  } 
  else
  {
    modid_arr = modid_arr_2_devices;
  } 
  
  rc = l3_multipath_run(unit,unit_2nd,in_port,vlan,nof_paths,out_port_arr,modid_arr);
  return rc;
}

int
l3_multipath_run_with_defaults_dvapi(int unit, int unit_2nd, int inPort, int outPort0, int outPort1, int outPort2, int outPort3)
{
  int rc;
  int in_port = inPort;
  int vlan = 12;
  int nof_paths = 4;
  int out_port_arr[4] = {outPort0,outPort1,outPort2,outPort3};
  int modid_arr_1_device[4] = {unit,unit,unit,unit};
  int modid_arr_2_devices[4] = {unit,unit,unit_2nd,unit_2nd};
  int *modid_arr;

  if (unit_2nd < 0)
  {
    modid_arr = modid_arr_1_device;
  } 
  else
  {
    modid_arr = modid_arr_2_devices;
  } 
  
  rc = l3_multipath_run(unit,unit_2nd,in_port,vlan,nof_paths,out_port_arr,modid_arr);
  return rc;
}